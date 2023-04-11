import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Menu 1.0 as M
import Controls 1.0 as Controls
import QCC.DataView.TableView 1.0 as DV
import QCC.Controls 1.0 as QCC
import QCC.Filters 2.0

import Dialogs 1.0 as Dialogs

import Theme 1.0
import Models.Log 1.0

import SSU 1.0

M.MenuPage {
    id: page

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    readonly property alias table: tableView

    Component.onCompleted: {
        userActivityLogModel.updateModelData()
    }

    header: RowLayout {
        QCC.SymbolButton {
            id: downloadButton
            Layout.leftMargin: Theme.indent
            enabled: !downloadMenu.dataLoading
            text: downloadMenu.dataLoading ? qsTr("Loading")
                                           : qsTr("Download")
            symbol: downloadMenu.dataLoading ? "\uf110"
                                             : "\uf0ed"
            onClicked: downloadMenu.open()
        }

        QCC.TextField {
            id: searchField
            Layout.fillWidth: true
            focus: true
            elevation: 2
            placeholderText: qsTr("Search")
        }

        QCC.SymbolButton {
            id: filterButton
            Layout.rightMargin: Theme.indent
            text: qsTr("Filters")
            symbol: "\uf0b0"
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
            text: qsTr("User activity log is empty")
        }
    }

    Controls.TableView {
        id: tableView
        visible: rows > 0
        width: parent.width; height: parent.height


        columnWidthProvider: function (column) { return column === UserActivityLogModel.OccurredTimeColumn ? Theme.pixelSize * 15
                                                                                                           : column === UserActivityLogModel.ActionColumn ? Theme.pixelSize * 20
                                                                                                                                                          : Theme.pixelSize * 10 }

        model: SortFilterProxyTableModel {
            sortOrder: tableView.sortIndicatorOrder
            sortCaseSensitivity: Qt.CaseInsensitive
            sortColumn: tableView.sortIndicatorColumn

            filterKeyColumn: -1
            filterWildcardString: "*" + searchField.text + "*"
            filterCaseSensitivity: Qt.CaseInsensitive

            sourceModel: UserActivityLogModel {
                id: userActivityLogModel
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
    }

    footer: Item {
        height: footerRow.height

        Row {
            id: footerRow
            anchors { right: parent.right; rightMargin: Theme.indent; }
            spacing: Theme.middleIndent
            visible: tableView.rows > 0

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Rows") + ": " + tableView.rows
            }

            DV.TableToolbar {
                id: tableToolbar
                anchors.verticalCenter: parent.verticalCenter
                popupMargins: Theme.indent
                tableView: tableView
                color: Material.accent
                printButtonActive: false
            }
        }
    }


    // Всплывающее окно фильтров таблицы..

    Menu {
        id: filterMenu
        width: Theme.pixelSize * 20
        x: parent.width - width;
        modal: true
        topPadding: Theme.indent; bottomPadding: Theme.indent
        leftPadding: Theme.middleIndent; rightPadding: Theme.middleIndent

        QCC.CheckableDateInput {
            id: startDateInput
            width: parent.width
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime(-3600)
            placeholderText: qsTr("From")   // Occurred time
            checked: true
            readOnly: !checked

            onDateChanged: userActivityLogModel.updateModelData()
        }

        QCC.CheckableDateInput {
            id: endDateInput
            width: parent.width
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime()
            placeholderText: qsTr("To")   // Occurred time
            checked: false
            readOnly: !checked

            onDateChanged: userActivityLogModel.updateModelData()
        }
    }   // Menu

    // всплывающее окно загрузки..
    Dialogs.DownloadDataMenu {
        id: downloadMenu
        width: Theme.pixelSize * 20
        onDownloadData: downloadConfirmation.open()
        dataLoading: device === null ? false
                                     : device.userActivityLogLoading

        Dialogs.ConfirmationPopup {
            id: downloadConfirmation
            text: qsTr("The download will take some time. Are you sure?")
            onClicked: device.downloadUserLog(downloadMenu.time1, downloadMenu.time2)
        }
    }
}


