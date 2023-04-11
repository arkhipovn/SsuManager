import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Dialogs 1.0 as Dialogs
import Controls 1.0 as Controls
import Theme 1.0

import QCC.Controls 1.0 as QCC
import Models 1.0

import SSU 1.0

Controls.StackPage {
    id: page

    property var device: null

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..


    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()

        QCC.SymbolButton {
            color: Material.accent
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: device.updateEventDescription()
        }
    }

    TabBar {
        id: tabBar
        width: parent.width

        readonly property int currentCardType: tabBar.cardTypes === null ? -1
                                                                         : tabBar.cardTypes[tabBar.currentIndex]
        readonly property var cardTypes: device === null ? null
                                                         : device.panel.cardTypes
        Repeater {
            model: tabBar.cardTypes === null ? null
                                             : tabBar.cardTypes.length

            TabButton {
                //                width: implicitWidth
                text: SSU.cardTypeToString(tabBar.cardTypes[modelData])
            }
        }
    }

    GroupBox {
        anchors { top: tabBar.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; topMargin: Theme.smallIndent; }
        Material.elevation: 3

        topPadding: 1; bottomPadding: 1
        leftPadding: 1; rightPadding: 1
        spacing: 0

        ListView {
            id: listView
            width: parent.width; height: parent.height

            clip: true
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.VerticalFlick
            headerPositioning: ListView.OverlayHeader

            interactive: height < contentHeight

            ScrollBar.vertical: ScrollBar { id: scrollbar; policy: ScrollBar.AlwaysOn; visible: listView.interactive }

            contentWidth: scrollbar.visible ? width - scrollbar.width
                                            : width

            model: EventDescriptionFilterProxyModel {
                cardType: tabBar.currentCardType
                sourceModel: device === null ? null
                                             : device.eventDescriptionList
            }

            header: headerDelegate
            delegate: eventDelegate
        }
    }





    Component {
        id: headerDelegate

        Controls.TableHeader {
            width: ListView.view.contentWidth
            Material.elevation: 1

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                text: qsTr("Id")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 6
                text: qsTr("Event")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                text: qsTr("Status")
                color: Material.accentColor
            }
        }
    }

    Component {
        id: eventDelegate

        Controls.RowLayoutPane {
            width: ListView.view.contentWidth
            seperatorActive: true
            spacing: 0

            backgroundColor: {
                programSettings.hasAlarmColorIndication
                return programSettings.alarmColorIndication(eventStatus)
            }

            backgroundContent: MouseArea {
                width: parent.width; height: parent.height
                onClicked: if(page.controlRights) statusPopupLoader.open(eventId, eventStatus, description)
            }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                text: eventId
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 6
                text: description
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                text: SSU.eventStatusToString(eventStatus)
                //                color: {
                //                    programSettings.hasAlarmColorIndication
                //                    return programSettings.alarmColorIndication(eventStatus)
                //                }
            }
        }
    }

    Loader {
        id: statusPopupLoader
        x: parent.width / 2 - width / 2
        active: false

        property int eventId: -1
        property int eventStatus: -1
        property string eventDescription: ""

        function open(id, status, description) {
            eventId = id
            eventStatus = status
            eventDescription = description
            active = true
        }

        sourceComponent: Dialogs.SettingsPopup {
            id: popup
            width: Theme.pixelSize * 20
            visible: true
            title: qsTr("Event status")
            onClosed: statusPopupLoader.active = false

            contentItem: Column {
                spacing: Theme.middleIndent

                Label {
                    width: parent.width
                    wrapMode: Label.WordWrap
                    text: statusPopupLoader.eventDescription
                }

                Controls.ComboBox {
                    id: statusComboBox
                    width: parent.width
                    focus: true
                    value: statusPopupLoader.eventStatus
                    model: Vch003.eventStatusList()
                    delegateTextFunc: SSU.eventStatusToString
                }

                Button {
                    anchors.right: parent.right
                    enabled: statusComboBox.value != statusPopupLoader.eventStatus
                    text: qsTr("Apply")
                    onClicked: {
                        device.changeEventStatus(tabBar.currentCardType, statusPopupLoader.eventId, statusComboBox.value)
                        popup.close()
                    }
                }
            }
        }
    }


}

