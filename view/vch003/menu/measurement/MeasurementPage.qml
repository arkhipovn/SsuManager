import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs
import Menu 1.0 as M

import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.DataView.TableView 1.0 as DV
import QCC.DataView.ChartViewExtension 1.0

import SSU 1.0

M.MenuPage {
    id: page

    property alias viewType: dataViewStackLayout.currentIndex

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    property var model: null
    property alias headerData: headerRow.data

    property int rows: 0

    Connections {
        target: page.model
        onRowsRemoved: rows = page.model.rowCount()
        onRowsInserted: rows = page.model.rowCount()
        onModelReset: rows = page.model.rowCount()
    }

    header: RowLayout {
        id: headerRow
        layoutDirection: Qt.RightToLeft

        QCC.SymbolButton {
            visible: page.rows > 0
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: Theme.indent
            text: qsTr("Masks")
            symbol: "\uf05e"
            onClicked: dataMaskPopup.open()

            Popup {
                id: dataMaskPopup
                y: parent.height
                width: Theme.pixelSize * 17
                topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent
                leftMargin: Theme.indent; rightMargin: Theme.indent
                modal: true

                contentItem: M.MaskComboBox {
                    Component.onCompleted: {
                        addSourceModel(controller.measMaskList)
                        if(device != null)
                            addSourceModel(device.thresholdList)
                    }

                    onCurrentMaskChanged: {
                        page.model.mask = mask
                        dataMaskPopup.close()
                    }
                }
            }
        }

        // Выбор отображения данных. На графике или таблица..

        QCC.SymbolButton {
            visible: page.rows > 0
            Layout.alignment: Qt.AlignVCenter
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            text: dataViewStackLayout.currentIndex == 1 ? qsTr("Table")
                                                        : qsTr("Chart")
            symbol: "\uf0d7"

            layoutDirection: Qt.RightToLeft

            onClicked: {
                if(mouse.button === Qt.RightButton) {
                    dataViewTypePopup.open()
                }
                else {
                    dataViewStackLayout.currentIndex = dataViewStackLayout.currentIndex == 1 ? 0 : 1
                }
            }

            toolTip.text: dataViewStackLayout.currentIndex == 1 ? qsTr("Click - to display on the graph")
                                                                : qsTr("Click - to display in the table")

            Popup {
                id: dataViewTypePopup
                width: Theme.pixelSize * 15; y: parent.height
                leftMargin: Theme.indent; rightMargin: Theme.indent
                padding: 0
                modal: true

                contentItem: Column {
                    Controls.IconItemDelegate {
                        width: parent.width
                        symbol: "\uf0ce"
                        symbolColor: Material.accentColor
                        text: qsTr("Table")
                        highlighted: dataViewStackLayout.currentIndex === 1
                        onClicked: {
                            dataViewStackLayout.currentIndex = 1
                            dataViewTypePopup.close()
                        }
                    }

                    Controls.IconItemDelegate {
                        width: parent.width
                        symbolColor: Material.accentColor
                        symbol: "\uf1fe"
                        text: qsTr("Chart")
                        highlighted: dataViewStackLayout.currentIndex === 0
                        onClicked: {
                            dataViewStackLayout.currentIndex = 0
                            dataViewTypePopup.close()
                        }
                    }
                }
            }
        }

        Item { Layout.fillWidth: true; }    // Чтобы занять свободное пространство

        Pane {
            visible: page.rows > 0
            Material.elevation: 1
            padding: 0
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: Theme.indent

            contentItem: Row {
                RadioButton {
                    topInset: 0; bottomInset: 0
                    topPadding: 6; bottomPadding: 6
                    anchors.verticalCenter: parent.verticalCenter
                    text: SSU.measDataTypeToString(SSU.Mtie)
                    checked: page.model == null ? false
                                                : page.model.dataType === SSU.Mtie
                    onClicked: {
                        if(page.model != null) {
                            page.model.dataType = SSU.Mtie
                        }
                    }
                }

                RadioButton {
                    topInset: 0; bottomInset: 0
                    topPadding: 6; bottomPadding: 6
                    anchors.verticalCenter: parent.verticalCenter
                    text: SSU.measDataTypeToString(SSU.Tdev)
                    checked: page.model == null ? false
                                                : page.model.dataType === SSU.Tdev
                    onClicked: {
                        if(page.model != null) {
                            page.model.dataType = SSU.Tdev
                        }
                    }
                }

                RadioButton {
                    topInset: 0; bottomInset: 0
                    topPadding: 6; bottomPadding: 6
                    anchors.verticalCenter: parent.verticalCenter
                    text: SSU.measDataTypeToString(SSU.Rf)
                    checked: page.model == null ? false
                                                : page.model.dataType === SSU.Rf
                    onClicked: {
                        if(page.model != null) {
                            page.model.dataType = SSU.Rf
                        }
                    }
                }
            }   // contentItem Pane
        }   // Pane select type meas
    }   // RowLayout header

    Controls.LoaderStackLayout {
        id: dataViewStackLayout
        width: parent.width; height: parent.height
        visible: page.rows > 0

        currentIndex: 1 // Таблица сначала

        Loader {
            active: false

            Layout.fillWidth: true; Layout.fillHeight: true

            sourceComponent: Item {
                MeasurementChartView {
                    id: measurementChartView
                    anchors { top: parent.top; bottom: measurementChartViewFooter.top; }
                    width: parent.width
                    model: page.model
                }

                ChartPrintToolbar {
                    id: measurementChartViewFooter
                    anchors { right: parent.right; bottom: parent.bottom; }
                    color: Material.accentColor
                    chartView: measurementChartView
                }
            }
        }

        Loader {
            active: false
            Layout.fillWidth: true; Layout.fillHeight: true

            sourceComponent: Item {
                MeasurementTableView {
                    id: table
                    anchors { top: parent.top; bottom: measurementTableViewFooter.top; }
                    width: parent.width
                    sourceModel: page.model
                }

                Row {
                    id: measurementTableViewFooter
                    anchors { right: parent.right; bottom: parent.bottom; }
                    spacing: Theme.middleIndent

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("Rows") + ": " + table.rows
                    }

                    DV.TableToolbar {
                        id: tableToolbar
                        tableView: table
                        color: Material.accent
                        printButtonActive: false
                    }
                }
            }
        }
    }   // StackLayout
}
