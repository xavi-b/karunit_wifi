#include "settingswidget.h"

void SettingsWidget::addAccessPoint(NetworkManager::AccessPoint const& accessPoint)
{
    emit log(QString("addAccessPoint"));
    // For simplification we use APs only with Wep security or without any security
    if (accessPoint.wpaFlags().testFlag(NetworkManager::AccessPoint::PairWep40) ||
        accessPoint.wpaFlags().testFlag(NetworkManager::AccessPoint::PairWep104) ||
        accessPoint.wpaFlags().testFlag(NetworkManager::AccessPoint::GroupWep40) ||
        accessPoint.wpaFlags().testFlag(NetworkManager::AccessPoint::GroupWep104) ||
        !accessPoint.wpaFlags())
    {
        emit log(QString("Flags OK"));
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(accessPoint.ssid());
        item->setData(Qt::UserRole, accessPoint.uni());
        item->setData(Qt::ForegroundRole, QColor((wifiDevice->activeAccessPoint() && accessPoint.uni() == wifiDevice->activeAccessPoint()->uni()) ? Qt::green : Qt::black));
        this->accessPointListWidget->addItem(item);
    }
}

void SettingsWidget::removeAccessPoint(QString const& uni)
{
    emit log(QString("removeAccessPoint"));
    for(int i = this->accessPointListWidget->count()-1; i >= 0; --i)
        if(this->accessPointListWidget->item(i)->data(Qt::UserRole).toString() == uni)
            delete this->accessPointListWidget->takeItem(i);
}

void SettingsWidget::updateActiveAccessPoint(QString const& uni)
{
    emit log(QString("updateActiveAccessPoint"));
    for(int i = 0; i < this->accessPointListWidget->count(); ++i)
    {
        if(this->accessPointListWidget->item(i)->data(Qt::UserRole).toString() == uni)
            this->accessPointListWidget->item(i)->setData(Qt::ForegroundRole, QColor(Qt::green));
        else
            this->accessPointListWidget->item(i)->setData(Qt::ForegroundRole, QColor(Qt::black));
    }
}

void SettingsWidget::connectTo(QString const& uni)
{
    if(this->wifiDevice->activeAccessPoint() && this->wifiDevice->activeAccessPoint()->uni() == uni)
        return;

    NetworkManager::AccessPoint::Ptr ap = this->wifiDevice->findAccessPoint(uni);

    if(!ap)
    {
        this->removeAccessPoint(uni);
        return;
    }

    NetworkManager::ConnectionSettings* settings = new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless);

    // Now we will prepare our new connection, we have to specify ID and create new UUID
    settings->setId(ap->ssid());
    settings->setUuid(QUuid::createUuid().toString().mid(1, QUuid::createUuid().toString().length() - 2));

    // For wireless setting we have to specify SSID
    NetworkManager::WirelessSetting::Ptr wirelessSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
    wirelessSetting->setSsid(ap->ssid().toUtf8());
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = settings->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
    ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Automatic);

    //FIXME for some reason availableConnections is sometimes empty
    emit log(QString("SSID: %1").arg(ap->ssid()));
    NetworkManager::Connection::Ptr c = nullptr;
    for(auto v : this->wifiDevice->availableConnections())
    {
        emit log(QString("Available connection: %1").arg(v->name()));
        if(v->name() == ap->ssid())
            c = v;
    }

    if(c != nullptr)
        NetworkManager::activateConnection(c->path(), this->wifiDevice->uni(), ap->uni());
    else
        NetworkManager::addAndActivateConnection(settings->toMap(), this->wifiDevice->uni(), ap->uni());
}

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;
    this->accessPointListWidget = new QListWidget;
    layout->addWidget(this->accessPointListWidget);
    this->scanBtn = new QPushButton(tr("Scan"));
    this->scanBtn->setDisabled(true);
    layout->addWidget(this->scanBtn);
    this->setLayout(layout);
}

SettingsWidget::~SettingsWidget()
{

}

void SettingsWidget::initialize()
{
    qDBusRegisterMetaType<NMVariantMapMap>();

    NetworkManager::Device::List deviceList = NetworkManager::networkInterfaces();

    emit log(QString("Device count: %1").arg(deviceList.size()));
    for(NetworkManager::Device::Ptr dev : deviceList)
    {
        emit log(QString("Device name: %1").arg(dev->udi()));
        if(dev->type() == NetworkManager::Device::Wifi)
        {
            emit log(QString("Wifi Device"));
            this->wifiDevice = qobject_cast<NetworkManager::WirelessDevice*>(dev);
            break;
        }
    }

    if(this->wifiDevice)
    {
        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::accessPointAppeared, this, [=](QString const& uni)
        {
            this->addAccessPoint(NetworkManager::AccessPoint(uni));
            this->updateActiveAccessPoint(this->wifiDevice->activeAccessPoint() ? this->wifiDevice->activeAccessPoint()->uni() : QString());
        });

        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::accessPointDisappeared, this, &SettingsWidget::removeAccessPoint);

        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &SettingsWidget::updateActiveAccessPoint);

        emit log(QString("AccesPoint count: %1").arg(this->wifiDevice->accessPoints().size()));
        for(auto ap : this->wifiDevice->accessPoints())
            this->addAccessPoint(NetworkManager::AccessPoint(ap));

        this->scanBtn->setEnabled(true);
        connect(this->scanBtn, &QPushButton::clicked, this, [=]()
        {
            this->scanBtn->setDisabled(true);
            QtConcurrent::run([=]()
            {
                auto reply = this->wifiDevice->requestScan();
                reply.waitForFinished();
                this->scanBtn->setEnabled(true);
            });
        });

        connect(this->accessPointListWidget, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem* item)
        {
            this->connectTo(item->data(Qt::UserRole).toString());
        });
    }
}
