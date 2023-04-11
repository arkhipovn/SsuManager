import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Menu 1.0 as M
import Controls 1.0 as Controls

import QCC.Controls 1.0 as QCC
import Theme 1.0

import SSU 1.0

M.MenuPage {
    padding: Theme.indent

    ColumnLayout {
        width: parent.width; height: parent.height
        spacing: Theme.indent

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: deviceInfoGroup.height

            QCC.VchIcon {
                height: parent.height; width: height
                animation.running: true
                animation.interval: 1000
                currentDateTime: true
                timeSpec: programSettings.timeSpec
            }

            GroupBox {
                id: deviceInfoGroup
                Material.elevation: 3
                anchors.right: parent.right
                width: parent.width * 0.67 * 0.67

                topPadding: 1; bottomPadding: 2
                leftPadding: 1; rightPadding: 1
                spacing: 0

                Column {
                    id: panelHeaderColumn
                    width: parent.width

                    // header
                    Controls.TableHeader {
                        width: parent.width
                        Material.elevation: 1

                        Controls.TableCell {
                            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                            color: Material.accentColor
                            text: qsTr("Device")
                        }

                        Controls.Separator { orientation: Qt.Vertical; }

                        Controls.TableCell {
                            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                            color: Material.accentColor
                            text: qsTr("Serial number")
                        }
                    }

                    // Panel serial number
                    Controls.RowLayoutPane {
                        width: parent.width;
                        seperatorActive: true
                        spacing: 0

                        Controls.TableCell {
                            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                            text: qsTr("Panel")
                        }

                        Controls.Separator { orientation: Qt.Vertical; }

                        Controls.TableCell {
                            Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                            text: device == null ? ""
                                                 : device.panel.serialNumber
                        }
                    }

                    Repeater {
                        readonly property var monitoringCard: device == null ? null
                                                                             : device.panel.inventory.monitoringCard
                        //                        model: monitoringCard === null ? null
                        //                                                       : [ monitoringCard.powerA, monitoringCard.powerB ]

                        model: {
                            if(monitoringCard === null) return null
                            var list = []
                            if(monitoringCard.powerA !== null && monitoringCard.powerA.cardActive)
                                list.push(monitoringCard.powerA)
                            if(monitoringCard.powerB !== null && monitoringCard.powerB.cardActive)
                                list.push(monitoringCard.powerB)
                            return list
                        }

                        Controls.RowLayoutPane {
                            width: parent.width;
                            seperatorActive: true
                            spacing: 0

                            Controls.TableCell {
                                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                                text: SSU.cardTypeToString(modelData.type) + " " + (index == 0 ? "A" : "B")
                            }

                            Controls.Separator { orientation: Qt.Vertical; }

                            Controls.TableCell {
                                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                                text: modelData.serialNumber
                            }
                        }
                    }
                }   // Column header
            }
        }

        GroupBox {
            Layout.fillHeight: true; Layout.fillWidth: true
            Material.elevation: 3

            topPadding: 1; bottomPadding: 1
            leftPadding: 1; rightPadding: 1
            spacing: 0

            M.InventoryTable {
                width: parent.width; height: parent.height
                model: device == null ? null
                                      : device.panel.inventory.data
            }
        }
    }

    footer: Controls.ToolBar {
        Material.elevation: 0

        Row {
            anchors.verticalCenter: parent.verticalCenter
            QCC.SymbolButton {
                anchors.verticalCenter: parent.verticalCenter
                symbol: "\uf0c7" // floppy-o
                toolTip.text: qsTr("Save to csv")
                color: Material.accentColor
                onClicked: device.panel.saveInventoryInformation()
            }

            QCC.SymbolButton {
                anchors.verticalCenter: parent.verticalCenter
                symbol: "\uf02f"    // print
                toolTip.text: qsTr("Print")
                color: Material.accentColor
                onClicked: device.panel.printInventoryInformation()
            }
        }

        QCC.SymbolButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: device.updateInventory()
        }
    }
}
