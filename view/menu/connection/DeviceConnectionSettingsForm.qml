import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QCC.Controls 1.0 as QCC

import Theme 1.0
import ConnectionSettings 1.0

/*
    Форма ввода хоста и порта
*/

Item {
    id: form

    property var device: null
    readonly property var connectionSettings: device == null ? null
                                                             : device.connectionSettings
    property bool waiting: false

    signal connectToHost(var address, var port)
    signal canceled()

    Keys.onReturnPressed: {
        if(connectButton.enabled)
            connectButton.method()
    }

    Label {
        id: label
        width: parent.width
        padding: Theme.indent
        wrapMode: Label.WordWrap
        text: device == null ? qsTr("SSU")
                             : device.name()
        color: Material.accent
        horizontalAlignment: Label.AlignHCenter
    }

    Column {
        anchors { top: label.bottom; topMargin: Theme.smallIndent; }
        width: parent.width

        QCC.TextField {
            id: addressField
            width: parent.width
            readOnly: waiting

            placeholderText: qsTr("Address")
            text: connectionSettings === null ? ""
                                              : connectionSettings.hostname
        }

        QCC.TextField {
            id: portField
            width: parent.width
            readOnly: waiting

            placeholderText: qsTr("Port")
            text: connectionSettings === null ? ""
                                              : connectionSettings.port
            validator: IntValidator {
                bottom: 0
            }
            message: acceptableInput ? "" : qsTr("Enter a value greater than 0")
        }

        QCC.SymbolButton {
            topInset: 0; bottomInset: 0
            checkable: true
            enabled: !waiting
            checked: connectionSettings === null ? false
                                                 : connectionSettings.autoConnect
            symbol: checked ? "\uf14a" : "\uf096"
            color: checked ? Material.accent : Material.backgroundDimColor
            onClicked: controller.setAutoConnect(form.device, checked)
            text: qsTr("Auto connect")
        }
    }

    Button {
        id: connectButton
        anchors { bottom: parent.bottom; right: parent.right; }

        text: connectionSettings !== null && connectionSettings.status === ConnectionSettings.Unconnected ? qsTr("Connect")
                                                                                                          : qsTr("Cancel")

        Material.background: connectionSettings !== null && connectionSettings.status === ConnectionSettings.Unconnected ? Material.Blue
                                                                                                                         : Material.Grey
        Material.foreground: "white"


        enabled: connectionSettings !== null && connectionSettings.status !== ConnectionSettings.Unconnected ? connectionSettings.status !== ConnectionSettings.Disconnection       // Если отключаемся, больше не даем нажимать
                                                                                                             : portField.acceptableInput && addressField.acceptableInput && addressField.text.length > 0

        function method() {
            if(connectionSettings.status === ConnectionSettings.Unconnected)
                form.connectToHost(addressField.text, portField.text)
            else
                form.canceled()
        }

        onClicked: method()
    }
}
