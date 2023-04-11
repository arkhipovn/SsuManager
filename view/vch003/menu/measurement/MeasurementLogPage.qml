import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs
import Menu 1.0 as M
import Models.Meas 1.0

import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.Filters 2.0
import QCC.DataView.TableView 1.0 as DV

M.MenuPage {
    id: page

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    property alias reclist: measLogModel.reclist

    Component.onCompleted: {
        measLogModel.updateModelData()
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

        QCC.CheckableDateInput {
            id: startDateInput
            Layout.alignment: Qt.AlignVCenter
            elevation: hovered ? 3 : 2
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime(-3600)
            placeholderText: qsTr("From")   // Occurred time
            checked: true
            readOnly: !checked

            onDateChanged: measLogModel.updateModelData()
        }

        QCC.CheckableDateInput {
            id: endDateInput
            Layout.alignment: Qt.AlignVCenter
            elevation: hovered ? 3 : 2
            format: programSettings.dateTimeFormat
            dateTime: programSettings.getCurrentDateTime()
            placeholderText: qsTr("To")   // Occurred time
            checked: false
            readOnly: !checked

            onDateChanged: measLogModel.updateModelData()
        }

        Controls.ComboBox {
            id: intervalComboBox
            Layout.alignment: Qt.AlignVCenter
            implicitWidth: Theme.pixelSize * 12
            model: [ 0, 1, 3, 6, 12, 24 ]
            displayText: currentIndex === 0 ? qsTr("Decimation")
                                            : (currentText + " " + qsTr("h"))
            delegateTextFunc: function(value) { return value === 0 ? qsTr("N/A")
                                                                   : (value + " " + qsTr("h")) }
            onCurrentIndexChanged: measLogModel.updateModelData()
        }

        Item { Layout.fillWidth: true; }    // Чтобы занять свободное пространство

        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            Layout.rightMargin: Theme.indent
            visible: table.rows > 0
            symbol: "\uf05e"
            text: qsTr("Masks")
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
                        measLogModel.mask = mask
                        dataMaskPopup.close()
                    }
                }
            }
        }
    }

    Loader {
        width: parent.width
        active: table.rows === 0
        anchors.verticalCenter: parent.verticalCenter
        sourceComponent: Label {
            id: label
            padding: Theme.indent
            wrapMode: Label.WordWrap
            horizontalAlignment: Qt.AlignHCenter

            //@N Как это лучше сделать?
            Connections {
                target: page.reclist
                onRowsRemoved: numberOfReclist = page.reclist.rowCount()
                onRowsInserted: numberOfReclist = page.reclist.rowCount()
                onModelReset: numberOfReclist = page.reclist.rowCount()
            }

            property int numberOfReclist: 0

            text: page.reclist === null ? ""
                                        : numberOfReclist > 0 ? qsTr("Select channels to show measurement data over a specified period of time")
                                                              : qsTr("No measurement data for the specified time period")
        }
    }

    Item {
        width: parent.width; height: parent.height
        visible: table.rows > 0

        MeasurementTableView {
            id: table
            anchors { top: parent.top; bottom: measurementTableViewFooter.top; }
            width: parent.width;

            sourceModel: MeasLogModel {
                id: measLogModel
                device: page.device
                shortHeaderTitles: false

                function updateModelData() {
                    if(!startDateInput.readOnly && !startDateInput.acceptableInput ||
                            !endDateInput.readOnly && !endDateInput.acceptableInput)
                        return
                    var start = startDateInput.readOnly ? new Date(NaN) : programSettings.dateTimeFromString(startDateInput.text)
                    var end = endDateInput.readOnly ? new Date(NaN) : programSettings.dateTimeFromString(endDateInput.text)
                    setInterval(start, end, intervalComboBox.model[intervalComboBox.currentIndex])
                }
            }

            onContextMenuRequested: {
                if(selectedIndexes.length === 0) return
                contextMenu.x = mouseX
                contextMenu.y = mouseY
                contextMenu.open()
            }
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

    Controls.ToolTip {
        id: warningToolTip
        text: qsTr("Too many selected rows")
        timeout: 3000
    }

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Show chart");
            onTriggered: {
                measurementsChartWindowLoader.active = true

                let selectedRows = table.selectionModel.selectedRows(0)
                if(selectedRows.length > 30) {
                    warningToolTip.open()
                    return
                }
                measurementsChartWindowLoader.setMeasList(table.model.indexesFromSource(selectedRows))
            }
        }
    }


    // загрузчик для отображения графика в отдельном окне
    Loader {
        id: measurementsChartWindowLoader
        active: false

        function setMeasList(indexes) {
            //@N тут не сразу же грузится.. Ждать нужно, наверное
            item.setMeasList(measLogModel.getMeasData(indexes))
        }

        sourceComponent: Dialogs.ApplicationWindow {
            id: channelSettingsDialog
            title: qsTr("Measurements")
            width: Math.min(80 * Theme.pixelSize, 1000); height: Math.min(50 * Theme.pixelSize, 1000)
            settingsCategory: "MeasurementsChartWindow"
            onClosing: measurementsChartWindowLoader.active = false

            function setMeasList(measList) {
                measurementsModel.set(measList)
            }

            MeasurementPage {
                width: parent.width; height: parent.height
                device: page.device
                viewType: 0 //@N Убрать магическое число..
                model: MeasurementsModel {
                    id: measurementsModel
                    device: page.device
                    shortHeaderTitles: false
                }
            }
        }
    }

    // всплывающее окно загрузки..
    Dialogs.DownloadDataMenu {
        id: downloadMenu
        width: Theme.pixelSize * 20
        onDownloadData: downloadConfirmation.open()
        dataLoading: device === null ? false
                                     : device.measLogLoading

        Dialogs.ConfirmationPopup {
            id: downloadConfirmation
            text: qsTr("The download will take some time. Are you sure?")
            onClicked: device.downloadReclist(downloadMenu.time1, downloadMenu.time2)
        }
    }


}
