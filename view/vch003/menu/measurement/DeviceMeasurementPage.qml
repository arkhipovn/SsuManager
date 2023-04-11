import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Menu 1.0 as M
import Theme 1.0

import Models 1.0

import SSU 1.0

M.MenuPage {
    id: page
    padding: 0

    Controls.LoaderStackLayout {
        id: pageLoader
        anchors { left: parent.left; right: reclistPane.left; }
        height: parent.height
        currentIndex: tabBar.currentIndex

        readonly property var reclist: currentItem === null ? null
                                                            : currentItem.active && currentItem.status === Loader.Ready ? (currentItem.item === null ? null
                                                                                                                                                     : currentItem.item.reclist)
                                                                                                                        : null
        readonly property var outputCard: device == null ? null
                                                         : device.panel.inventory.hasData ? device.panel.inventory.findCard(SSU.Output)
                                                                                          : null

        Loader {
            id: currentMeasurementPageLoader
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            Binding {
                target: currentMeasurementPageLoader
                when: pageLoader.outputCard === null
                property: "active"
                value: false
            }

            sourceComponent: CurrentMeasurementPage {
                device: page.device
            }
        }

        Loader {
            id: measurementLogPage
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: MeasurementLogPage {
                device: page.device
            }
        }
    }

    Controls.ToolBar {
        anchors { bottom: parent.bottom; left: parent.left; }
        Material.elevation: 0

        TabBar {
            id: tabBar
            anchors.verticalCenter: parent.verticalCenter
            currentIndex: pageLoader.outputCard === null ? 1
                                                         : 0

            //            Binding {
            //                target: tabBar
            //                when: pageLoader.outputCard === null
            //                property: "currentIndex"
            //                value: 1         // Перейдем на страницу истории
            //            }

            TabButton {
                enabled: pageLoader.outputCard !== null
                text: qsTr("Current measurements")
            }

            TabButton {
                width: implicitWidth
                text: qsTr("Measurements history")
            }
        }   // TabBar
    }   // footer

    Pane {
        id: reclistPane
        visible: channelListView.count > 0
        anchors.right: parent.right
        width: visible ? Math.min(Theme.pixelSize * 15, parent.width * 0.2)
                       : 0
        height: parent.height
        padding: 0
        Material.elevation: 3

        contentItem: ListView {
            id: channelListView
            spacing: 0
            model: pageLoader.reclist
            interactive: height < contentHeight

            header: Label {
                text: qsTr("Channel list")
                color: Material.accentColor
                topPadding: Theme.indent; bottomPadding: Theme.indent
                leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
            }

            delegate: ItemDelegate {
                id: delegate
                width: ListView.view.width
                leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                text: device == null ? ""
                                     : device.channelDescription.cardInputChannelToString(SSU.Output, channel)
                highlighted: selected
                checkable: true
                checked: selected
                onClicked: selected = checked
            }

            footer: Column {
                width: ListView.view.width
                visible: channelListView.model !== null

                Controls.Separator { orientation: Qt.Horizontal; }

                ItemDelegate {
                    width: parent.width
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                    text: qsTr("Select All")
                    onClicked: channelListView.model.selectAll(true)
                }

                ItemDelegate {
                    width: parent.width
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                    text: qsTr("Unselect all")
                    onClicked: channelListView.model.selectAll(false)
                }
            }
        }
    }
}
