import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import Models.Port 1.0

import Theme 1.0
import Strings 1.0

GridLayout {
    id: ptpPage

    property int networkConfigurationIndex: -1
    property var ptpModel: null

    rowSpacing: Theme.smallIndent

    Rectangle {
        Layout.row: 0; Layout.column: 0
        Layout.columnSpan: 2
        Layout.fillWidth: true;
        implicitHeight: 1;
        color: Material.dropShadowColor;
    }

    Label {
        Layout.row: 1; Layout.column: 0
        leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
        text: qsTr("PTP")
        color: Material.accentColor
        font.bold: true
    }

    Loader {
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.row: 1; Layout.column: 1
        Layout.rowSpan: 2

        active: controlRights
        visible: active

        sourceComponent: QCC.LinkButton {
            text: qsTr("Remove")
            color: Material.color(Material.Red)
            onClicked: removeConfPopup.open()

            Dialogs.ConfirmationPopup {
                id: removeConfPopup
                text: qsTr("Are you sure you want to delete the PTP configuration?")
                onClicked: device.send(card.clearPtpConfigurationCommand(portModel, networkConfigurationIndex))
            }
        }
    }

    GridLayout {
        Layout.row: 2; Layout.column: 0

        rowSpacing: Theme.middleIndent
        columnSpacing: Theme.middleIndent

        Controls.Group {
            Layout.column: 0; Layout.row: 0
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignTop

            title: qsTr("Configuration")

            onClicked: {
                if(ptpModel != null && controlRights) {
                    controlPopup.openPopup(ptpConfigurationComponent, -1, qsTr("PTP"), true)
                }
            }

            contentItem: Grid {
                columns: 2
                spacing: Theme.middleIndent

                Label {
                    text: qsTr("PTP Mode")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.ptpModeToString(ptpModel.ptpMode)
                }

                Label {
                    text: qsTr("Clock ID")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null || ptpModel.clockId.length === 0 ? "-"
                                                                            : ptpModel.clockId
                }

                Label {
                    text: qsTr("Tx Mode")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.txModeToString(ptpModel.txMode)
                }

                Label {
                    text: qsTr("Way Mode")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.wayModeToString(ptpModel.wayMode)
                }

                Label {
                    text: qsTr("Sync Mode")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.syncModeToString(ptpModel.syncMode)
                }

                Label {
                    text: qsTr("Delay Mechanism")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.delayMechanismToString(ptpModel.delayMechanism)
                }

                Label {
                    text: qsTr("Domain Number")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.domainNumber
                }

                Label {
                    text: qsTr("Protocol")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : Strings.protocolToString(ptpModel.protocol)
                }
            }
        }   // Group

        Controls.Group {
            Layout.column: 1; Layout.row: 0
            Layout.alignment: Qt.AlignTop

            title: qsTr("Intervals")

            onClicked: {
                if(ptpModel != null && controlRights) {
                    controlPopup.openPopup(ptpConfigurationComponent, -1, qsTr("PTP"), true)
                }
            }

            contentItem: Grid {
                columns: 2
                spacing: Theme.middleIndent

                Label {
                    text: qsTr("Announce Timeout")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.announceTimeout
                }

                Label {
                    text: qsTr("Sync")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.syncInterval
                }

                Label {
                    text: qsTr("Delay Request")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.delayRequest
                }

                Label {
                    text: qsTr("PDelay Request")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.pDelayRequest
                }
            }
        }   // Group

        Controls.Group {
            Layout.column: 1; Layout.row: 1
            Layout.alignment: Qt.AlignTop

            title: qsTr("Announce")

            onClicked: {
                if(ptpModel != null && controlRights) {
                    controlPopup.openPopup(ptpConfigurationComponent, -1, qsTr("PTP"), true)
                }
            }

            contentItem: Grid {
                columns: 2
                spacing: Theme.middleIndent

                Label {
                    text: qsTr("Priority 1")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.priority1
                }

                Label {
                    text: qsTr("Priority 2")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.priority2
                }

                Label {
                    text: qsTr("Clock Class")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null ? "-"
                                           : ptpModel.clockClass
                }
            }
        }   // Group


        Controls.Group {
            Layout.column: 0; Layout.row: 2
            Layout.alignment: Qt.AlignTop

            visible: ptpModel != null && ptpModel.ptpMode === Ptp.Slave

            title: Strings.ptpModeToString(Ptp.Slave)

            onClicked: {
                if(ptpModel != null && controlRights) {
                    controlPopup.openPopup(ptpConfigurationComponent, -1, qsTr("PTP"), true)
                }
            }

            contentItem: Grid {
                columns: 2
                spacing: Theme.middleIndent

                Label {
                    text: qsTr("Remote master IPv4")
                    font.bold: true
                }

                Label {
                    text: ptpModel == null || ptpModel.remoteMasterAddress.length === 0 ? "-"
                                                                                        : ptpModel.remoteMasterAddress
                }
            }
        }   // Group
    }

    Component {
        id: ptpConfigurationComponent
        PtpConfiguration {
            ptpModel: ptpPage.ntpModel
            networkConfigurationIndex: ptpPage.networkConfigurationIndex
        }
    }
}
