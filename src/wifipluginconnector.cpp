#include "wifipluginconnector.h"

KU_Wifi_PluginConnector::KU_Wifi_PluginConnector(QObject* parent)
    : KU::PLUGIN::PluginConnector(parent)
{
    connect(this, &KU_Wifi_PluginConnector::scanningChanged, this, [=]() {
        if (scanning)
            emitLogSignal(QString("Start scanning"));
        else
            emitLogSignal(QString("Scanning finished"));
    });
}

void KU_Wifi_PluginConnector::initialize()
{
    qDBusRegisterMetaType<NMVariantMapMap>();

    NetworkManager::Device::List deviceList = NetworkManager::networkInterfaces();

    emitLogSignal(QString("Device count: %1").arg(deviceList.size()));
    for (NetworkManager::Device::Ptr dev : deviceList)
    {
        emitLogSignal(QString("Device name: %1").arg(dev->udi()));
        if (dev->type() == NetworkManager::Device::Wifi)
        {
            emitLogSignal(QString("Wifi Device"));
            this->wifiDevice = qobject_cast<NetworkManager::WirelessDevice*>(dev);
            break;
        }
    }

    if (this->wifiDevice)
    {
        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::accessPointAppeared, this, [=](QString const& uni) {
            this->addAccessPoint(NetworkManager::AccessPoint(uni));
            this->updateActiveAccessPoint(this->wifiDevice->activeAccessPoint() ? this->wifiDevice->activeAccessPoint()->uni() : QString());
        });

        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::accessPointDisappeared, this, &KU_Wifi_PluginConnector::removeAccessPoint);

        connect(this->wifiDevice.get(), &NetworkManager::WirelessDevice::activeAccessPointChanged, this, &KU_Wifi_PluginConnector::updateActiveAccessPoint);

        emitLogSignal(QString("AccessPoint count: %1").arg(this->wifiDevice->accessPoints().size()));
        for (auto ap : this->wifiDevice->accessPoints())
            this->addAccessPoint(NetworkManager::AccessPoint(ap));
    }
}

void KU_Wifi_PluginConnector::scan()
{
    this->scanning = true;
    emit scanningChanged();
    QtConcurrent::run([=]() {
        auto reply = this->wifiDevice->requestScan();
        reply.waitForFinished();
        this->scanning = false;
        emit scanningChanged();
    });
}

void KU_Wifi_PluginConnector::addAccessPoint(NetworkManager::AccessPoint const& accessPoint)
{
    emitLogSignal(QString("addAccessPoint"));
    emitLogSignal(accessPoint.ssid());
    qDebug() << accessPoint.wpaFlags();

    emitLogSignal(QString("Flags OK"));

    AccessPoint a;
    a.ssid      = accessPoint.ssid();
    a.connected = wifiDevice->activeAccessPoint() && accessPoint.uni() == wifiDevice->activeAccessPoint()->uni();

    this->accessPoints[accessPoint.uni()] = a;
}

void KU_Wifi_PluginConnector::removeAccessPoint(QString const& uni)
{
    emitLogSignal(QString("removeAccessPoint"));
    if (this->accessPoints.contains(uni))
        emitLogSignal(this->accessPoints[uni].ssid);

    this->accessPoints.remove(uni);
    emit accessPointsChanged();
}

void KU_Wifi_PluginConnector::updateActiveAccessPoint(QString const& uni)
{
    emitLogSignal(QString("updateActiveAccessPoint"));
    if (this->accessPoints.contains(uni))
        emitLogSignal(this->accessPoints[uni].ssid);

    for (auto& e : this->accessPoints.keys())
        this->accessPoints[e].connected = e == uni;
    emit accessPointsChanged();
}

// https://code.woboq.org/qt5/kf5/networkmanager-qt/examples/createconnection/main.cpp.html
void KU_Wifi_PluginConnector::connectTo(QString const& uni)
{
    if (this->wifiDevice->activeAccessPoint() && this->wifiDevice->activeAccessPoint()->uni() == uni)
        return;

    NetworkManager::AccessPoint::Ptr ap = this->wifiDevice->findAccessPoint(uni);

    if (!ap)
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

    // FIXME for some reason availableConnections is sometimes empty
    emit log(QString("SSID: %1").arg(ap->ssid()));

    NetworkManager::Connection::Ptr c = nullptr;
    for (auto v : this->wifiDevice->availableConnections())
    {
        emitLogSignal(QString("Available connection: %1").arg(v->name()));
        if (v->name() == ap->ssid())
            c = v;
    }

    if (c != nullptr)
        NetworkManager::activateConnection(c->path(), this->wifiDevice->uni(), ap->uni());
    else
        NetworkManager::addAndActivateConnection(settings->toMap(), this->wifiDevice->uni(), ap->uni());
}

bool KU_Wifi_PluginConnector::isConnected(const QString& uni)
{
    if (this->accessPoints.contains(uni))
        return this->accessPoints[uni].connected;

    return false;
}

QString KU_Wifi_PluginConnector::ssid(const QString& uni)
{
    if (this->accessPoints.contains(uni))
        return this->accessPoints[uni].ssid;

    return QString();
}

QStringList KU_Wifi_PluginConnector::accessPointsAliases() const
{
    return this->accessPoints.keys();
}
