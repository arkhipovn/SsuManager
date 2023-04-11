import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import SSU 1.0
import Theme 1.0

import Models 1.0
import QCC.Filters 2.0

RowLayout {
    id: form
    spacing: Theme.indent
    property alias model: sortFilterProxyTableModel.sourceModel

    function updateList() {
        device.send(card.updateSatelliteListCommand())
    }

    Component.onCompleted: form.updateList()

    Timer {
        interval: Math.max(30000, programSettings.pollingInterval * 4)
        running: true
        repeat: true
        onTriggered: form.updateList()
    }

    ColumnLayout {
        Layout.fillWidth: true; Layout.fillHeight: true
        Layout.preferredWidth: 3
        spacing: Theme.middleIndent

        Flow {
            id: filtersRow
            Layout.fillWidth: true

            function setTrackedFilter() {
                if(!trackedVisibleItem.checked && !notTrackedVisibleItem.checked)
                    sortFilterProxyTableModel.filters.removeColumnFilter(SatelliteListModel.SatTracked)
                else if(trackedVisibleItem.checked) {
                    sortFilterProxyTableModel.filters.set(SatelliteListModel.SatTracked, "", false)
                }
                else if(notTrackedVisibleItem.checked) {
                    sortFilterProxyTableModel.filters.set(SatelliteListModel.SatTracked, "", true)
                }
            }

            QCC.SymbolButton {
                //                anchors.verticalCenter: parent.verticalCenter
                topInset: 0; bottomInset: 0
                text: qsTr("Update")
                symbol: "\uf021"
                toolTip.text: qsTr("Update satellite list")
                onClicked: form.updateList()
            }

            ItemDelegate {
                id: trackedVisibleItem
                //                anchors.verticalCenter: parent.verticalCenter
                implicitHeight: Theme.rowHeight
                topInset: 0; bottomInset: 0
                topPadding: 0; bottomPadding: 0
                text: qsTr("Tracked")
                checkable: true
                checked: false
                highlighted: checked
                onClicked: {
                    if(checked) {
                        if(notTrackedVisibleItem.checked)
                            notTrackedVisibleItem.checked = false
                    }
                    filtersRow.setTrackedFilter()
                }
            }


            ItemDelegate {
                id: notTrackedVisibleItem
                //                anchors.verticalCenter: parent.verticalCenter
                implicitHeight: Theme.rowHeight
                topInset: 0; bottomInset: 0
                topPadding: 0; bottomPadding: 0
                text: qsTr("Not tracked")
                checkable: true
                checked: false
                highlighted: checked
                onClicked: {
                    if(checked) {
                        if(trackedVisibleItem.checked)
                            trackedVisibleItem.checked = false
                    }
                    filtersRow.setTrackedFilter()
                }
            }

            Controls.ComboBox {
                //                anchors.verticalCenter: parent.verticalCenter
                implicitHeight: Theme.rowHeight; implicitWidth: Theme.pixelSize * 10
                topInset: 0; bottomInset: 0
                topPadding: 0; bottomPadding: 0
                currentIndex: 0
                model: [ "", SSU.GPS, SSU.GLONASS, SSU.BeiDou, SSU.Galileo ]
                displayText: currentIndex == 0 ? qsTr("All systems") : SSU.satelliteSystemToString(currentText)
                delegateTextFunc: function(value) { return value === "" ? qsTr("All systems") : SSU.satelliteSystemToString(value) }

                onActivated: {
                    if(currentIndex == 0)
                        sortFilterProxyTableModel.filters.removeRoleFilter(sortFilterProxyTableModel.roleKey("type"))
                    else
                        sortFilterProxyTableModel.filters.set(0, sortFilterProxyTableModel.roleKey("type"), model[currentIndex])
                }
            }
        }

        Controls.TableView {
            id: gnssTableView
            Layout.fillWidth: true; Layout.fillHeight: true

            columnWidthProvider: function (column) {
                switch(column) {
                case SatelliteListModel.SatId:
                case SatelliteListModel.SatTracked:
                    return Theme.pixelSize * 6
                case SatelliteListModel.SatType:
                    return Theme.pixelSize * 10
                default: break
                }
                return Theme.pixelSize * 9
            }

            //@N это сделал из-за того, что пока не знаю как обновить selectedRows в SkyView
            onSortIndicatorColumnChanged: clearSelection()
            onSortIndicatorOrderChanged: clearSelection()

            model: SortFilterProxyTableModel {
                id: sortFilterProxyTableModel
                sortOrder: gnssTableView.sortIndicatorOrder
                sortCaseSensitivity: Qt.CaseInsensitive
                sortColumn: gnssTableView.sortIndicatorColumn
            }
        }
    }

    SkyView {
        id: skyView
        Layout.fillWidth: true; Layout.fillHeight: true
        Layout.preferredWidth: 2
        labelFontSize: Theme.pixelSize * 0.8

        model: sortFilterProxyTableModel

        selectedRows: {
            gnssTableView.selectionModel.hasSelection
            var list = []
            var selectedRows = gnssTableView.selectionModel.selectedRows(0)
            for(var i = 0; i < selectedRows.length; ++i)
                list.push(selectedRows[i].row)
            return list
        }
    }


}   // RowLayout
