import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC

import Dialogs 1.0 as Dialogs
import Controls 1.0 as Controls

import Theme 1.0

import "../menu/"
import "../menu/events"

CardPage {
    id: page

    topPadding: 0; bottomPadding: 0
    leftPadding: 0; rightPadding: 0

    StackView {
        id: stackView
        width: parent.width; height: parent.height
        initialItem: deviceSettingsMenuComponent

        Connections {
            target: stackView.depth == 1 ? null
                                         : stackView.currentItem
            onComeback: stackView.pop()
        }
    }

    Loader {
        id: deviceNamePopupLoader
        x: parent.width / 2 - width / 2
        active: false
        sourceComponent: Dialogs.SettingsPopup {
            id: popup
            visible: true
            title: qsTr("Device name")
            onClosed: deviceNamePopupLoader.active = false

            contentItem: Column {
                TextField {
                    id: textArea
                    width: parent.width
                    focus: true
                    selectByMouse: true
                    wrapMode: TextArea.Wrap
                    text: card.deviceName
                }

                Button {
                    anchors.right: parent.right;
                    text: qsTr("Apply")
                    onClicked: {
                        device.send(card.commandToSetDeviceName(textArea.text))
                        popup.close()
                    }
                }
            }
        }
    }

    Component {
        id: deviceSettingsMenuComponent

        Controls.StackPage {
            leftPadding: 0; rightPadding: 0

            header: Controls.StackPageHeader {
                leftPadding: 0; rightPadding: 0
                contentItem: ItemDelegate {
                    leftPadding: Theme.indent; rightPadding: Theme.indent
                    text: card.deviceName
                    enabled: device == null ? false
                                            : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
                    onClicked: deviceNamePopupLoader.active = true
                }
            }

            Pane {
                width: parent.width
                Material.elevation: 1
                padding: 0

                contentItem: Column {
                    Controls.IconItemDelegate {
                        text: qsTr("Event statuses settings")
                        width: parent.width
                        symbol: "\uf0cb"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Green)
                        onClicked: stackView.push(eventListStackPageComponent, { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Network configuration")
                        width: parent.width
                        symbol: "\uf0ac"
                        symbolScale: 1.2
                        symbolColor: Material.color(Material.Blue)
                        onClicked: stackView.push(networkConfigStackPageComponent, { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Date & Time")
                        width: parent.width
                        symbol: "\uf073" //"\uf017"
                        //                        symbolScale: 1.2
                        symbolColor: Material.color(Material.Orange)
                        onClicked: stackView.push(dateConfigStackPageComponent, { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Device data management")
                        width: parent.width
                        visible: device == null ? false
                                                : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
                        symbol: "\uf1c0"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Teal)
                        onClicked: stackView.push(dataManagementStackPageComponent, { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Device update")
                        width: parent.width
                        visible: device == null ? false
                                                : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
                        symbol: "\uf021"
                        symbolColor: Material.color(Material.BlueGrey)
                        onClicked: stackView.push(deviceUpdateStackPageComponent, { title: text })
                    }
                }
            }
        }
    }   // deviceSettingsMenuComponent

    Component {
        id: eventListStackPageComponent
        EventListStackPage {
            device: page.device
        }
    }

    Component {
        id: dataManagementStackPageComponent
        DeviceDataManagementStackPage {
            device: page.device
        }
    }

    Component {
        id: deviceUpdateStackPageComponent
        DeviceUpdateStackPage {
            device: page.device
        }
    }

    Component {
        id: dateConfigStackPageComponent
        DateConfigStackPage {
            device: page.device
        }
    }

    Component {
        id: networkConfigStackPageComponent
        NetworkConfigStackPage {
            device: page.device
        }
    }
}
