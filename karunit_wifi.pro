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
INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include/
INCLUDEPATH += /usr/lib/aarch64-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/lib/arm-linux-gnueabi/glib-2.0/include/
INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include/
INCLUDEPATH += /usr/lib/i386-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include/
INCLUDEPATH += /usr/include/libnm/

LIBS += -L$$PWD/../karunit/plugininterface/ -lplugininterface
INCLUDEPATH += $$PWD/../karunit/plugininterface

LIBS += -L$$PWD/../karunit/common/ -lcommon
INCLUDEPATH += $$PWD/../karunit/common

LIBS += -L$$PWD/../karunit/third-party/xblog/lib -lxblog
INCLUDEPATH += $$PWD/../karunit/third-party/xblog/include

SUBDIRS += \
    src/

include(src/src.pri)
