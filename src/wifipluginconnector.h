#ifndef WIFIPLUGINCONNECTOR_H
#define WIFIPLUGINCONNECTOR_H

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
#include "plugininterface.h"

class KU_Wifi_PluginConnector : public KU::PLUGIN::PluginConnector
{
    struct AccessPoint
    {
        QString ssid;
        bool    connected = false;
    };

    Q_OBJECT

    Q_PROPERTY(bool scanning MEMBER scanning NOTIFY scanningChanged)
    Q_PROPERTY(QStringList accessPointsAliases READ accessPointsAliases NOTIFY accessPointsChanged)
public:
    KU_Wifi_PluginConnector(QObject* parent = nullptr);

    void initialize();

    Q_INVOKABLE void    scan();
    Q_INVOKABLE void    connectTo(QString const& uni);
    Q_INVOKABLE bool    isConnected(QString const& uni);
    Q_INVOKABLE QString ssid(QString const& uni);

    QStringList accessPointsAliases() const;

signals:
    void scanningChanged();
    void accessPointsChanged();

private:
    bool                                scanning = false;
    NetworkManager::WirelessDevice::Ptr wifiDevice;
    QMap<QString, AccessPoint>          accessPoints;

    void addAccessPoint(NetworkManager::AccessPoint const& accessPoint);
    void removeAccessPoint(QString const& uni);
    void updateActiveAccessPoint(QString const& uni);
};

#endif // WIFIPLUGINCONNECTOR_H
