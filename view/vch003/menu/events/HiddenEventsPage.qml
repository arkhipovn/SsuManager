import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Menu 1.0 as M
import Models.Log 1.0

import QCC.Controls 1.0 as QCC
import QCC.Filters 2.0

import Theme 1.0

M.MenuPage {
    id: page

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    Component.onCompleted: hiddenEventModel.update()

    Controls.TableView {
        id: tableView
        visible: rows > 0
        width: parent.width; height: parent.height
        columnWidthProvider: function (column) {
            switch(column) {
            case HiddenEventModel.OccurredTimeColumn:
            case HiddenEventModel.ClearedTimeColumn:
            case HiddenEventModel.CommentColumn:
                return 0
            case EventLogModel.CardTypeColumn:
                return Theme.pixelSize * 15
            case HiddenEventModel.SlotColumn:
            case ActiveEventModel.EventIdColumn:
                return Theme.pixelSize * 5
            case HiddenEventModel.DescriptionColumn:
                return Theme.pixelSize * 25
            default: break
            }
            return Theme.pixelSize * 10
        }

        model: SortFilterProxyTableModel {
            sortOrder: tableView.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortColumn: tableView.sortIndicatorColumn

            sourceModel: HiddenEventModel {
                id: hiddenEventModel
                device: page.device
            }
        }

        onContextMenuRequested: {
            if(selectedIndexes.length === 0) return
            eventMaskingMenu.x = mouseX
            eventMaskingMenu.y = mouseY
            eventMaskingMenu.open()
        }
    }

    function showSelectedEvents() {
        let selectedRows = tableView.selectionModel.selectedRows(0)
        hiddenEventModel.showEvents(tableView.model.indexesFromSource(selectedRows))
    }

    Menu {
        id: eventMaskingMenu

        MenuItem {
            text: qsTr("Show events")
            onClicked: page.showSelectedEvents()
        }
    }

    footer: Controls.ToolBar {
        Material.elevation: 0

        Row {
            spacing: Theme.middleIndent
            Button {
                enabled: tableView.selectedIndexes.length > 0
                text: qsTr("Show selected")
                onClicked: page.showSelectedEvents()
            }

            Button {
                enabled: tableView.rows > 0
                text: qsTr("Show all")
                onClicked: {
                    hiddenEventModel.showAllEvents()
                }
            }
        }
    }
}
