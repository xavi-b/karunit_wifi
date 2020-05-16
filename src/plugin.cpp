#include "plugin.h"

KU_Wifi_Plugin::~KU_Wifi_Plugin()
{

}

QString KU_Wifi_Plugin::name() const
{
    return "Wifi";
}

QString KU_Wifi_Plugin::id() const
{
    return "wifi";
}

KU::PLUGIN::PluginVersion KU_Wifi_Plugin::version() const
{
    return { 1, 0, 0 };
}

QString KU_Wifi_Plugin::license() const
{
    return "LGPL";
}

QIcon KU_Wifi_Plugin::icon() const
{
    return QIcon();
}

bool KU_Wifi_Plugin::initialize()
{
    this->settingsWidget = new SettingsWidget;
    QObject::connect(this->settingsWidget, &SettingsWidget::log,
                     this->getPluginConnector(), &KU::PLUGIN::PluginConnector::log);
    return true;
}

bool KU_Wifi_Plugin::stop()
{
    return true;
}

QWidget* KU_Wifi_Plugin::createWidget()
{
    return nullptr;
}

QWidget* KU_Wifi_Plugin::createSettingsWidget()
{
    return this->settingsWidget;
}

bool KU_Wifi_Plugin::loadSettings()
{
    return true;
}

bool KU_Wifi_Plugin::saveSettings() const
{
    return KU::Settings::instance()->status() == QSettings::NoError;
}
