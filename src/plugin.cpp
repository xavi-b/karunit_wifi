#include "plugin.h"
#include <QQmlEngine>

QString KU_Wifi_Plugin::name() const
{
    return "Wifi";
}

QString KU_Wifi_Plugin::id() const
{
    return "karunit_wifi";
}

KU::PLUGIN::PluginVersion KU_Wifi_Plugin::version() const
{
    return {1, 0, 0};
}

QString KU_Wifi_Plugin::license() const
{
    return "LGPL";
}

QString KU_Wifi_Plugin::icon() const
{
    return QString();
}

bool KU_Wifi_Plugin::initialize()
{
    qmlRegisterSingletonInstance("KarunitPlugins", 1, 0, "KUPWifiPluginConnector", this->pluginConnector);
    this->getPluginConnector()->initialize();

    return true;
}

bool KU_Wifi_Plugin::stop()
{
    return true;
}

bool KU_Wifi_Plugin::loadSettings()
{
    return true;
}

bool KU_Wifi_Plugin::saveSettings()
{
    return KU::Settings::instance()->status() == QSettings::NoError;
}

KU_Wifi_PluginConnector* KU_Wifi_Plugin::getPluginConnector()
{
    if (this->pluginConnector == nullptr)
        this->pluginConnector = new KU_Wifi_PluginConnector;
    return qobject_cast<KU_Wifi_PluginConnector*>(this->pluginConnector);
}
