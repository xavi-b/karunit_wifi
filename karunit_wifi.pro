TEMPLATE        = lib
CONFIG         += plugin c++17
DEFINES        += QT_DEPRECATED_WARNINGS
QT             += core widgets dbus network
TARGET          = karunit_wifi_plugin
DESTDIR         = $$PWD/../karunit/app/plugins

LIBS += -lKF5NetworkManagerQt
INCLUDEPATH += /usr/include/KF5/NetworkManagerQt
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/lib/glib-2.0/include
INCLUDEPATH += /usr/include/libnm/

LIBS += -L$$PWD/../karunit/plugininterface/ -lplugininterface
INCLUDEPATH += $$PWD/../karunit/plugininterface

LIBS += -L$$PWD/../karunit/common/ -lcommon
INCLUDEPATH += $$PWD/../karunit/common

LIBS += -L$$PWD/../karunit/third-party/xblog/ -lxblog
INCLUDEPATH += $$PWD/../karunit/third-party/xblog/src

SUBDIRS += \
    src/

include(src/src.pri)
