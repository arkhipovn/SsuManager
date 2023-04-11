import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QCC.Controls 1.0 as QCC

import Theme 1.0

/*
    Форма подключенного устройства. Отображаются логин и пароль. Есть возможность отключитьс
*/

Item {
    id: form

    property var device: null
    readonly property var connectionSettings: device == null ? null
                                                             : device.connectionSettings
    property bool waiting: false

    signal disconnectFromHost();

    Label {
        id: label
        padding: Theme.indent
        width: parent.width
        wrapMode: Label.WordWrap
        text: qsTr("Connection established")// qsTr("CONNECTION ESTABLISHED")
        font.capitalization: Font.AllUppercase
        horizontalAlignment: Label.AlignHCenter
        Material.foreground: Material.Green
    }

    Column {
        anchors { top: label.bottom; topMargin: Theme.smallIndent; }
        width: parent.width

        QCC.TextField {
            width: parent.width
            placeholderText: qsTr("Hostname")
            text: connectionSettings === null ? ""
                                              : connectionSettings.hostname
            readOnly: true
        }

        QCC.TextField {
            width: parent.width
            placeholderText: qsTr("Username")
            text: connectionSettings === null ? ""
                                              : connectionSettings.username
            readOnly: true
        }

        QCC.SymbolButton {
            topInset: 0; bottomInset: 0
            checkable: true
            checked: connectionSettings === null ? false
                                                 : connectionSettings.autoConnect
            symbol: checked ? "\uf14a" : "\uf096"
            color: checked ? Material.accent : Material.backgroundDimColor
            onClicked: controller.setAutoConnect(form.device, checked)
            text: qsTr("Auto connect")
        }
    }

    Button {
        anchors { bottom: parent.bottom; /*horizontalCenter: parent.horizontalCenter;*/ right: parent.right; }
        enabled: connectionSettings !== null && !waiting
        text: qsTr("Disconnect")
        Material.background: Material.Red
        Material.foreground: "white"
        onClicked: form.disconnectFromHost()
    }
}


