import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Menu 1.0 as M

import QCC.DataView.TableView 1.0 as DV

import Theme 1.0

M.MenuPage {
    id: page
    padding: 0

    function showEvents(status) {
        tabBar.currentIndex = 0 // Active.. //@N убрать магическое число
        pageLoader.itemAt(0).item.setStatusFilter(status)
    }

    Controls.LoaderStackLayout {
        id: pageLoader
        width: parent.width; height: parent.height
        currentIndex: tabBar.currentIndex

        readonly property var table: currentItem === null ? null
                                                          : currentItem.status === Loader.Ready ? (currentItem.item === null ? null
                                                                                                                             : currentItem.item.table)
                                                                                                : null
        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: ActiveEventsPage {
                device: page.device
            }
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: EventLogPage {
                device: page.device
            }
        }
    }

    footer: Controls.ToolBar {
        Material.elevation: 0

        TabBar {
            id: tabBar
            anchors.verticalCenter: parent.verticalCenter

            TabButton {
                width: implicitWidth
                text: qsTr("Active events")
            }

            TabButton {
                width: implicitWidth
                text: qsTr("Event history")
            }
        }   // TabBar

        Row {
            anchors { right: parent.right; verticalCenter: parent.verticalCenter; }
            spacing: Theme.middleIndent
            visible: pageLoader.table === null ? false : pageLoader.table.rows > 0

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Rows") + ": " + (pageLoader.table === null ? "" : pageLoader.table.rows)
            }

            DV.TableToolbar {
                id: tableToolbar
                tableView: pageLoader.table
                color: Material.accent
                printButtonActive: false
            }
        }
    }

}

