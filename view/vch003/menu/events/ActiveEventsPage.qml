import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Menu 1.0 as M
import Models.Log 1.0
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC
import QCC.Filters 2.0

import Theme 1.0

M.MenuPage {
    id: page

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    readonly property alias table: tableView

    function setStatusFilter(status) {
        filterMenu.setStatusFilter(status)
    }

    header: RowLayout {
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: Theme.indent
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: device.updateActiveEvents()
        }

        QCC.SuggestionsTextField {
            id: searchField
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            fixedHeight: true
            focus: true
            elevation: 2
            placeholderText: qsTr("Search by description")
            maximumHeight: page.height
            model: device == null ? []
                                  : device.eventDescriptionList === null ? []
                                                                         : device.eventDescriptionList.descriptions
        }

        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Masking")
            symbol: "\uf2a8"
            highlighted: hiddenEventsPopupLoader.active
            onClicked: hiddenEventsPopupLoader.active = true
        }

        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: Theme.indent
            text: qsTr("Filters")
            symbol: "\uf0b0"
            highlighted: filterMenu.opened
            onClicked: filterMenu.open()
        }
    }

    Loader {
        width: parent.width
        active: tableView.rows === 0
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: Label {
            id: label
            padding: Theme.indent
            wrapMode: Label.WordWrap
            horizontalAlignment: Qt.AlignHCenter
            text: qsTr("No active events")
        }
    }

    Controls.TableView {
        id: tableView
        visible: rows > 0
        width: parent.width; height: parent.height

        //        delegate: Label {
        //            text: tableView.cellTextProvider(styleData.columnIndex, styleData.value)
        //            color: styleData.textColor
        //            wrapMode: tableView.wrapMode
        //            verticalAlignment: Label.AlignVCenter
        //            padding: font.pixelSize
        //            elide: tableView.elide

        //            // выделение
        //            Rectangle {
        //                width: parent.width; height: parent.height
        //                opacity: styleData.selected ? 0.2 : 0.0
        //                color: styleData.currentIndexSelectedItem === styleData.currentIndex ? styleData.textColor : styleData.selectedColor
        //            }
        //                            //                        onRowIndexChanged: {
        //                            //                        }
        //        }

        model: SortFilterProxyTableModel {
            sortOrder: tableView.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortColumn: tableView.sortIndicatorColumn

            filterKeyColumn: EventLogModel.DescriptionColumn
            filterWildcardString: "*" + searchField.text + "*"
            filterCaseSensitivity: Qt.CaseInsensitive

            sourceModel: device == null ? null
                                        : device.activeEvents
        }

        columnWidthProvider: function (column) {
            switch(column) {
            case ActiveEventModel.OccurredTimeColumn:
                return Theme.pixelSize * 13
            case ActiveEventModel.ClearedTimeColumn:
                return 0
            case EventLogModel.CardTypeColumn:
                return Theme.pixelSize * 15
            case ActiveEventModel.SlotColumn:
            case ActiveEventModel.EventIdColumn:
                return Theme.pixelSize * 5
            case ActiveEventModel.DescriptionColumn:
                return Theme.pixelSize * 25
            default: break
            }
            return Theme.pixelSize * 10
        }

        onDoubleClicked: {
            let index = tableView.model.mapToSource(modelIndex)
            commentPopupLoader.open(device.activeEvents.hash(index.row),
                                    device.activeEvents.comment(index.row))
        }

        onContextMenuRequested: {
            if(selectedIndexes.length === 0) return
            eventMaskingMenu.x = mouseX
            eventMaskingMenu.y = mouseY
            eventMaskingMenu.open()
        }

        Menu {
            id: eventMaskingMenu

            MenuItem {
                text: qsTr("Hide events")
                onClicked: {
                    let selectedRows = tableView.selectionModel.selectedRows(0)
                    device.activeEvents.hideEvents(tableView.model.indexesFromSource(selectedRows))
                }
            }
        }
    }

    Loader {
        id: hiddenEventsPopupLoader
        width: parent.width; height: parent.height
        active: false
        onLoaded: item.open()
        sourceComponent: Dialogs.SettingsPopup {
            width: parent.width; height: parent.height
            title: qsTr("Masking")
            onClosed: hiddenEventsPopupLoader.active = false

            leftPadding: 0; rightPadding: 0; bottomPadding: 0

            contentItem: HiddenEventsPage {
                device: page.device
            }
        }
    }

    // Всплывающее окно фильтров таблицы..

    EventFilterList {
        id: filterMenu
        x: parent.width - width
        width: Math.min(Theme.pixelSize * 30, parent.width)
        height: parent.height

        device: page.device
        filters: tableView.model.filters
        sourceModel: tableView.model
    }

    Loader {
        id: commentPopupLoader
        x: parent.width / 2 - width / 2
        active: false

        property string comment: ""
        property var hash: 0

        function open(hash, comment) {
            if(hash.length === 0)
                return
            this.hash = hash
            this.comment = comment
            active = true
        }

        onActiveChanged: {
            if(!active) {
                comment = ""
                hash = ""
            }
        }

        sourceComponent: Dialogs.SettingsPopup {
            id: popup
            width: Math.min(Theme.pixelSize * 50, page.width * 0.6)
            title: qsTr("Comment")
            visible: true
            onClosed: commentPopupLoader.active = false

            contentItem: Column {
                TextArea {
                    id: textArea
                    width: parent.width
                    focus: true
                    text: commentPopupLoader.comment
                    selectByMouse: true
                    wrapMode: TextArea.Wrap
                }

                Button {
                    enabled: textArea.text !== commentPopupLoader.comment
                    anchors.right: parent.right;
                    text: qsTr("Apply")
                    onClicked: {
                        device.activeEvents.setComment(commentPopupLoader.hash, textArea.text)
                        popup.close()
                    }
                }
            }
        }
    }

}
