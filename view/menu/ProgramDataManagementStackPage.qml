import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Theme 1.0

import QtQuick.Layouts 1.12
import QCC.Controls 1.0 as QCC
import QCC.Fonts 1.0

import Models 1.0
import SSU 1.0

import Dialogs 1.0 as Dialogs


Controls.StackPage {
    id: page
    property var device: null
    leftPadding: 0; rightPadding: 0

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()

        QCC.SymbolButton {
            Layout.leftMargin: Theme.indent
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: programDataManagementModel.update()
        }
    }

    ListView {
        width: parent.width; height: parent.height
        model: ProgramDataManagementModel {
            id: programDataManagementModel
            path: SSU.programDataPath()
        }

        delegate: ItemDelegate {
            leftPadding: Theme.indent; rightPadding: Theme.indent
            width: ListView.view.width

            //@N Вот это переделать!! Нужно будет завязываться на серийном номере корзины
            readonly property bool isCurrent: device == null ? false
                                                             : name === device.connectionSettings.name()

            onClicked: controlPopup.openPopup(name, isCurrent)

            contentItem: RowLayout {
                Column {
                    Layout.fillWidth: true
                    spacing: 2

                    Label {
                        text: name
                        width: parent.width
                        elide: Qt.ElideRight
                    }

                    Label {
                        text: Qt.formatDateTime(lastUpdate, programSettings.dateTimeFormat)
                        width: parent.width
                        elide: Qt.ElideRight
                        color: Qt.lighter(Material.foreground, 2)
                        font.pixelSize: Theme.pixelSize * 0.8
                    }
                }

                Label {
                    visible: isCurrent
                    text: qsTr("Current")
                    Layout.leftMargin: Theme.indent
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    color: Material.color(Material.LightGreen)
                }

                Label {
                    Layout.leftMargin: isCurrent ? Theme.smallIndent
                                                 : Theme.indent
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    text: dataSize
                    color: Qt.lighter(Material.foreground, 2)
                }

                Label {
                    Layout.leftMargin: Theme.smallIndent
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    font.family: CFontLoader.awesome.name
                    text: "\uf105"
                }
            }
        }
    }

    Controls.ToolTip {
        id: databaseDeletionErrorToolTip
        delay: 0
        text: qsTr("Deletion is not possible. Database in use")
    }

    Dialogs.SettingsPopup {
        id: controlPopup

        function openPopup(device, isCurrent) {
            this.device = device
            currentDevice = isCurrent
            open()
        }

        property string device: ""
        property bool currentDevice: false

        leftPadding: 0; rightPadding: 0
        title: device

        contentItem: Column {
            spacing: 0

            Label {
                width: parent.width
                horizontalAlignment: Qt.AlignHCenter
                wrapMode: Label.WordWrap
                leftPadding: Theme.indent; rightPadding: Theme.indent

                text: qsTr("The data will be deleted from Your computer. If necessary, data can be downloaded from the device")

                topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent
                color: "white"
                background: Rectangle { color: Material.dropShadowColor }
            }

            ItemDelegate {
                width: parent.width
                text: qsTr("Show in explorer");
                onClicked: {
                    programDataManagementModel.openDir(controlPopup.device)
                }
            }

            Loader {
                width: parent.width; height: active ? implicitHeight : 0
                active: device !== null && controlPopup.currentDevice      // Можно удалить только у текущего устройства

                sourceComponent: Column {
                    //@N Потом нужно будет подумать насчет того, какие таблицы можно очищать? Вдруг будет другая версия и там другие таблицы..
                    ItemDelegate {
                        width: parent.width
                        text: qsTr("Clear event log")
                        onClicked: {
                            device.clearEventsLogTable()
                            programDataManagementModel.updateFileInfo(controlPopup.device)
                        }
                    }

                    ItemDelegate {
                        width: parent.width
                        text: qsTr("Clear user activity log")
                        onClicked: {
                            device.clearUserActivityLogTable()
                            programDataManagementModel.updateFileInfo(controlPopup.device)
                        }
                    }

                    ItemDelegate {
                        width: parent.width
                        text: qsTr("Clear measurement data")
                        onClicked: {
                            device.clearMeasTable()
                            programDataManagementModel.updateFileInfo(controlPopup.device)
                        }
                    }
                }
            }

            ItemDelegate {
                width: parent.width
                text: qsTr("Delete all")
                Material.foreground: Material.Red
                onClicked: {
                    if(!programDataManagementModel.remove(controlPopup.device))
                        databaseDeletionErrorToolTip.open()
                    controlPopup.close()
                }
            }
        }
    }

}

