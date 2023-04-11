import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import ConnectionSettings 1.0
import QCC.Controls 1.0 as QCC

import Theme 1.0

/*
    Форма ввода логина и пароля
*/

Item {
    id: form

    property var device: null
    readonly property var connectionSettings: device == null ? null
                                                             : device.connectionSettings
    property bool waiting: false

    signal authorization(var user, var password, var save)
    signal disconnectFromHost();

    readonly property bool onlyPassword: connectionSettings === null ? true
                                                                     : connectionSettings.status === ConnectionSettings.WaitingForPassword   // нужно ввести только пароль.. Тк пользователь уже подключился

    Keys.onReturnPressed: {
        if(connectButton.enabled)
            connectButton.method()
    }

    Column {
        width: parent.width
        spacing: Theme.smallIndent

        Label {
            padding: Theme.indent
            width: parent.width
            wrapMode: Label.WordWrap
            color: Material.accent
            text: onlyPassword ? (connectionSettings === null ? ""
                                                              : qsTr("Welcome") + " " + connectionSettings.username)
                               : (device == null ? ""
                                                 : qsTr("Sign in to") + " " + device.name())    //LOG IN
            font.capitalization: Font.AllUppercase
            horizontalAlignment: Label.AlignHCenter
        }

        Label {
            visible: onlyPassword
            width: parent.width; height: visible ? undefined : 0
            wrapMode: Label.WordWrap
            text: qsTr("Wrong password. Try again")
            horizontalAlignment: Label.AlignHCenter
        }

        Column {
            width: parent.width;

            QCC.TextField {
                id: usernameField
                visible: !onlyPassword
                width: parent.width; height: visible ? undefined : 0

                placeholderText: qsTr("Username")
                text: connectionSettings === null ? ""
                                                  : connectionSettings.username
                readOnly: waiting
            }

            QCC.PasswordTextField {
                id: passwordField
                width: parent.width
                text: connectionSettings === null ? ""
                                                  : connectionSettings.password
                readOnly: waiting
            }

            QCC.SymbolButton {
                id: storeUserInformationButton
                topInset: 0; bottomInset: 0
                enabled: !waiting
                checkable: true
                checked: connectionSettings === null ? false
                                                     : connectionSettings.storeUserInformation
                symbol: checked ? "\uf14a" : "\uf096"
                color: checked ? Material.accent : Material.backgroundDimColor

                text: qsTr("Remember")
            }
        }   // Column
    }   // Column

    Row {
        anchors { bottom: parent.bottom; right: parent.right }
        spacing: Theme.middleIndent;
        enabled: connectionSettings !== null

        Button {
            text: qsTr("Cancel")//qsTr("Disconnect")
            onClicked: form.disconnectFromHost()
            Material.background: Material.Grey
            Material.foreground: "white"
        }

        Button {
            id: connectButton
            enabled: usernameField.acceptableInput && usernameField.text.length > 0 && passwordField.text.length > 0 && !waiting
            text: qsTr("Sign in")
            Material.background: Material.Blue
            Material.foreground: "white"

            function method() {
                form.authorization(usernameField.text, passwordField.text, storeUserInformationButton.checked)
            }

            onClicked: method()
        }
    }
}


