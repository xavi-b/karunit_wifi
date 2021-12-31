import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Karunit 1.0
import KarunitPlugins 1.0

Item {
    anchors.fill: parent

    ListView {
        anchors.fill: parent
        id: listview
        model: KUPWifiPluginConnector.accessPointsAliases
        spacing: 5

        delegate: ItemDelegate {
            width: listview.width

            spacing: 5

            contentItem: Label {
                text: KUPWifiPluginConnector.ssid(modelData)
                color: KUPWifiPluginConnector.isConnected(
                           modelData) ? "green" : "red"
            }

            background: Rectangle {
                anchors.fill: parent
                color: "lightgrey"
            }

            onDoubleClicked: {
                KUPWifiPluginConnector.connectTo(modelData)
            }
        }

        RoundButton {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5
            enabled: !KUPWifiPluginConnector.scanning
            icon.name: "view-refresh"
            icon.color: "transparent"
            onClicked: {
                KUPWifiPluginConnector.scan()
            }
        }
    }
}
