import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Device 1.0 as D
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

D.CardPage {
    footerData: [
        Loader {
            Layout.alignment: Qt.AlignVCenter
            active: device == null ? false
                                   : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
            sourceComponent: Pane {
                padding: 0
                Material.elevation: 1

                contentItem: Row {
                    QCC.SymbolButton {
                        topInset: 0; bottomInset: 0
                        squareBackground: false
                        symbol: "\uf1da" // "\uf0a8", f187, f122
                        toolTip.text: qsTr("Restore default card settings")
                        onClicked: resetToDefaultCardCheckPasswordPopup.open()
                    }
                    QCC.SymbolButton {
                        topInset: 0; bottomInset: 0
                        squareBackground: false
                        symbol: "\uf01e" //"\uf0e2"
                        toolTip.text: qsTr("Reboot card")
                        onClicked: resetCardCheckPasswordPopup.open()
                    }
                }
            }
        },
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            topInset: 0; bottomInset: 0
            text: qsTr("Update")
            symbol: "\uf021"
            toolTip.text: qsTr("Update card parameters")
            onClicked: {
                device.sendList(card.updateCardParametersCommand())
                // device.send(card.updateCardParametersCommand())
            }
        },
        Item { Layout.fillWidth: true; }
    ]

    Dialogs.CheckPasswordPopup {
        id: resetToDefaultCardCheckPasswordPopup
        title: qsTr("Are you sure you want to reset the settings to default?")
        text: qsTr("To confirm the action, enter the password again")
        applyButtonText: qsTr("Restore")
        password: device == null ? ""
                                 : device.connectionSettings.password
        onClicked: device.resetToDefaultCard(card.slot)
    }

    Dialogs.CheckPasswordPopup {
        id: resetCardCheckPasswordPopup
        title: qsTr("Are you sure you want to reload the card?")
        text: qsTr("To confirm the action, enter the password again")
        applyButtonText: qsTr("Reload")
        password: device == null ? ""
                                 : device.connectionSettings.password
        onClicked: device.resetCard(card.slot)
    }
}
