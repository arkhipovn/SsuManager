import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC
import Theme 1.0

import Qt.labs.settings 1.0

import SSU 1.0
import ConnectionSettings 1.0









import QCC.Fonts 1.0
import "Menu/"
import "Menu/mask/"
import "vch003/menu/"
import "vch003/"
import QCC.Controls 2.0 as QCC2
import Controls 1.0 as Controls
import Menu 1.0 as Menu
import QtQuick.Layouts 1.12

import QCC.DataView.ChartViewExtension 1.0
import QtCharts 2.3
import QCC.DataView.TableView 1.0 as DV
import Models 1.0
import Models.Mask 1.0
import Models.Log 1.0
//import TestModels 1.0
//import AppUpdater 1.0
import "vch003/menu/measurement/"
import "vch003/pages/"
import QCC.Filters 2.0
import Strings 1.0


Dialogs.ApplicationWindow {
    id: appWindow
    width: Math.min(90 * Theme.pixelSize, 1000); height: Math.min(50 * Theme.pixelSize, 1000)
    title: qsTr("Manager")
    settingsCategory: "MainWindow"

    Settings {
        id: settingsObject
        fileName: SSU.programSettingsFile()
        category: SSU.windowSettingsGroup()
        property real scale: Theme.scale
        Component.onCompleted: Theme.scale = scale  // 1 раз считаем..
    }

    onClosing: {
        controller.closeEvent()
    }

    QCC.ToolTipListArea {
        id: windowToolTipList
        anchors { top: parent.top; bottom: parent.bottom; margins: Theme.indent; horizontalCenter: parent.horizontalCenter; }
        width: Math.max(Math.min(parent.width, parent.width * 0.4, Theme.pixelSize * 40), Theme.pixelSize * 15)
        timeout: 5000
        listType: 1 //QCC.ToolTipListModel.LatterDoNotRepeat

        function view(device, text, background, foreground = "white") {
            open(device.connectionSettings.hostname + ": " + text, background, foreground)
        }
    }

    Loader {
        width: parent.width; height: parent.height
        sourceComponent: panelFormComponent
    }

    Loader {
        active: SSU.kImitationWork ? false
                                   : controller === null ||
                                     controller.device === null ||
                                     controller.device.connectionSettings === null ? true
                                                                                   : controller.device.connectionSettings.status !== ConnectionSettings.Connected &&
                                                                                     controller.device.connectionSettings.status !== ConnectionSettings.Disconnection

        anchors { right: parent.right; bottom: parent.bottom; margins: Theme.indent; }
        sourceComponent: QCC.VchIcon {
            width: Theme.pixelSize * 8; height: width
            currentDateTime: true
            animation.running: true
        }
    }

    Component {
        id: panelFormComponent

        DevicePageLoader {
            id: devicePageLoader
            device: controller === null ? null
                                        : controller.device

            Connections {
                target: devicePageLoader.device

                onTrap: {
                    let str = slot
                    if(atEnd) {
                        str += " " + qsTr("Finished")
                    }
                    str += ". " + description

                    windowToolTipList.view(devicePageLoader.device, str,
                                           atEnd ? Material.color(Material.Green, Material.Shade800)
                                                 : programSettings.alarmColorIndication(status),
                                           atEnd ? "white"
                                                 : Material.foreground)
                }
            }

            Connections {
                target: device == null ? null
                                       : device.connectionSettings

                onDeviceConnected: windowToolTipList.view(device, qsTr("Device connected"), Material.color(Material.Green, Material.Shade800))
                onDeviceDisconnected: windowToolTipList.view(device, qsTr("Device disconnected"), Material.color(Material.Red, Material.Shade700))
                onDeviceNotResponding:  windowToolTipList.view(device, qsTr("Device not responding"), Material.color(Material.Orange, Material.Shade900))
                onConnectionNotEstablished: windowToolTipList.view(device, qsTr("Connection not established"), Material.color(Material.Orange, Material.Shade900))
                onInvalidUsername: {
                    if(device != null) {
                        device.connectionSettings.clearPassword()
                    }
                    windowToolTipList.view(device, qsTr("Wrong username"), Material.color(Material.Orange, Material.Shade900))
                }
                onInvalidPassword: {
                    if(device != null) {
                        device.connectionSettings.clearPassword()
                    }
                    windowToolTipList.view(device, qsTr("Wrong password"), Material.color(Material.Orange, Material.Shade900))
                }
                onUserBlocked: windowToolTipList.view(device, qsTr("The user is blocked"), Material.color(Material.Orange, Material.Shade900))
            }
        }
    }

}
