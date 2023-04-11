import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import Dialogs 1.0 as Dialogs

import Theme 1.0

import Models 1.0
import Models.Meas 1.0

import QtQml 2.12       // import QtQml.Models 2.14
import SSU 1.0

MeasurementPage {
    id: page

    property alias reclist: currentMeasurementsModel.reclist

    headerData: [
        Loader {
            active: device == null ? false
                                   : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
            visible: active
            Layout.alignment: Qt.AlignVCenter
            sourceComponent: QCC.SymbolButton {
                text: qsTr("Reset meas")
                symbol: "\uf0e2"
                color: Material.color(Material.Red)
                //                onClicked: resetMeasurementsCheckPasswordPopup.open()
                onClicked: resetMeasMenu.open()

                Menu {
                    id: resetMeasMenu
                    y: parent.height

                    Action {
                        text: qsTr("Reset all measurements")
                        onTriggered: {
                            resetMeasurementsCheckPasswordPopup.channel = -1
                            resetMeasurementsCheckPasswordPopup.open()
                        }
                    }

                    Menu {
                        id: contextMenu
                        title: qsTr("Reset channel measurements")

                        Instantiator {
                            model: currentMeasurementsModel.reclist

                            delegate: Action {
                                text: device == null ? ""
                                                     : device.channelDescription.cardInputChannelToString(SSU.Output, channel)
                                onTriggered: {
                                    resetMeasurementsCheckPasswordPopup.channel = channel
                                    resetMeasurementsCheckPasswordPopup.open()
                                }
                            }

                            onObjectAdded: contextMenu.insertAction(index, object)
                            onObjectRemoved: contextMenu.removeAction(object)
                        }
                    }
                }

                // Сброс измерений
                Dialogs.CheckPasswordPopup {
                    id: resetMeasurementsCheckPasswordPopup

                    property int channel: -1

                    title: qsTr("Are you sure you want to ask for measurements?")
                    text: qsTr("Measurement calculation will start over")

                    applyButtonText: qsTr("Yes")
                    password: device == null ? ""
                                             : device.connectionSettings.password
                    onClicked: device.resetMeasurements(channel)
                }
            }
        },
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            Layout.leftMargin: Theme.indent
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: device.updateCurrentMeas()
        }
    ]   // headerData

    Loader {
        width: parent.width
        active: page.rows === 0
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
                                        : numberOfReclist > 0 ? qsTr("Select channels to show measurement data")
                                                              : qsTr("No measurement data available at the moment")
        }
    }

    // Модель данных
    model: CurrentMeasurementsModel {
        id: currentMeasurementsModel
        device: page.device
        shortHeaderTitles: true
    }

    Component.onCompleted: {
        model.update()
    }
}
