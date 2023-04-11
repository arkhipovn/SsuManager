import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Menu 1.0 as M
import Models 1.0
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC
import QCC.Filters 2.0

import Theme 1.0

import SSU 1.0
import Models.Log 1.0

M.MenuPage {
    id: page

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    readonly property alias table: tableView

    function setStatusFilter(status) {
        filterMenu.setStatusFilter(status)
    }

    Component.onCompleted: {
        eventLogModel.updateModelData()
    }

    header: RowLayout {
        QCC.SymbolButton {
            id: downloadButton
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: Theme.indent
            enabled: !downloadMenu.dataLoading
            text: downloadMenu.dataLoading ? qsTr("Loading")
                                           : qsTr("Download")
            symbol: downloadMenu.dataLoading ? "\uf110"
                                             : "\uf0ed"
            onClicked: downloadMenu.open()
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
            text: qsTr("No events")
        }
    }

    Controls.TableView {
        id: tableView
        visible: rows > 0
        width: parent.width; height: parent.height

        columnWidthProvider: function (column) {
            switch(column) {
            case EventLogModel.OccurredTimeColumn:
            case EventLogModel.ClearedTimeColumn:
                return Theme.pixelSize * 13
            case EventLogModel.CardTypeColumn:
                return Theme.pixelSize * 15
            case EventLogModel.SlotColumn:
                return Theme.pixelSize * 5
            case ActiveEventModel.EventIdColumn:
                return Theme.pixelSize * 5
            case EventLogModel.DescriptionColumn:
                return Theme.pixelSize * 25
            default: break
            }
            return Theme.pixelSize * 10
        }

        model: SortFilterProxyTableModel {
            sortOrder: tableView.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortColumn: tableView.sortIndicatorColumn

            filterKeyColumn: EventLogModel.DescriptionColumn
            filterWildcardString: "*" + searchField.text + "*"
            filterCaseSensitivity: Qt.CaseInsensitive

            sourceModel: EventLogModel {
                id: eventLogModel
                device: page.device

                function updateModelData() {
                    if(!startDateInput.readOnly && !startDateInput.acceptableInput ||
                            !endDateInput.readOnly && !endDateInput.acceptableInput)
                        return
                    var start = startDateInput.readOnly ? new Date(NaN) : programSettings.dateTimeFromString(startDateInput.text)
                    var end = endDateInput.readOnly ? new Date(NaN) : programSettings.dateTimeFromString(endDateInput.text)
                    setInterval(start, end)
                }
            }
        }

        onDoubleClicked: {
            let index = tableView.model.mapToSource(modelIndex)
            commentPopupLoader.open(eventLogModel.hash(index.row),
                                    eventLogModel.comment(index.row))
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

        //        onFiltersCleared: {
        //            //            let updateModelData = false
        //            //            let startTime = programSettings.getCurrentDateTime(-3600)
        //            //            let endTime =  programSettings.getCurrentDateTime()
        //            //            if(startDateInput.checked != true || startDateInput.dateTime !== startTime) {
        //            //                startDateInput.checked = true
        //            //                startDateInput.dateTime = startTime
        //            //                updateModelData = true
        //            //            }
        //            //            if(endDateInput.checked != true || endDateInput.dateTime !== endTime) {
        //            //                endDateInput.checked = false
        //            //                endDateInput.dateTime = endTime
        //            //                updateModelData = true
        //            //            }
        //            //            if(updateModelData) {   // Если нужно, то обновим все..
        //            //                eventLogModel.updateModelData()
        //            //            }
        //            tableView.model.clearFilters()
        //        }

        QCC.CheckableDateInput {
            id: startDateInput
            width: parent.width
            elevation: hovered ? 3 : 2
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime(-3600)
            placeholderText: qsTr("From")   // Occurred time
            checked: true
            readOnly: !checked
            onDateChanged: eventLogModel.updateModelData()
        }

        QCC.CheckableDateInput {
            id: endDateInput
            width: parent.width
            elevation: hovered ? 3 : 2
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime()
            placeholderText: qsTr("To")   // Occurred time
            checked: false
            readOnly: !checked
            onDateChanged: eventLogModel.updateModelData()
        }

        MenuSeparator { width: parent.width; }
    }

    // всплывающее окно загрузки..
    Dialogs.DownloadDataMenu {
        id: downloadMenu
        width: Theme.pixelSize * 20
        onDownloadData: downloadConfirmation.open()
        dataLoading: device === null ? false
                                     : device.eventLogLoading

        Dialogs.ConfirmationPopup {
            id: downloadConfirmation
            text: qsTr("The download will take some time. Are you sure?")
            onClicked: device.downloadEventLog(downloadMenu.time1, downloadMenu.time2)
        }
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
                        eventLogModel.setComment(commentPopupLoader.hash, textArea.text)
                        popup.close()
                    }
                }
            }
        }
    }

}
