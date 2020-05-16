#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QVBoxLayout>
#include <arpa/inet.h>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <QDBusMetaType>
#include <QUuid>
#include <QtConcurrent/QtConcurrent>
#include <QListWidget>
#include <QPushButton>

class SettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget* accessPointListWidget;
    NetworkManager::WirelessDevice::Ptr wifiDevice;

    void addAccessPoint(NetworkManager::AccessPoint const& accessPoint);
    void removeAccessPoint(QString const& uni);
    void updateActiveAccessPoint(QString const& uni);
    void connectTo(QString const& uni);

public:
    SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

signals:
    void log(QString const& log);
};

#endif // SETTINGSWIDGET_H
