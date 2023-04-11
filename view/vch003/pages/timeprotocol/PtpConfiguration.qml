import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import QCC.Validators 1.0

import Controls 1.0 as Controls

import Models.Port 1.0

import Theme 1.0
import Strings 1.0

Column {
    property var ptpModel: null
    property int networkConfigurationIndex: -1

    spacing: Theme.middleIndent

    enum Profiles {
        Custom,
        P8265_1,
        P8275_1,
        P8275_2
    }

    Label {
        Layout.column: 0; Layout.row: 0
        Layout.columnSpan: 2
        text: qsTr("PTP Mode")
        font.bold: true
    }

    RowLayout {
        id: ptpModeLayout
        width: parent.width

        // Есть ли мастер
        readonly property int portSlaveIndex: {
            for(var i = 0; i < portModel.networkConfigurationList.length; ++i) {
                var ptp = portModel.networkConfigurationList[i].ptp
                if(ptp.ptpMode === Ptp.Slave)
                    return i
            }
            return -1
        }

        readonly property int ptpMode: masterRadioButton.checked ? Ptp.Master
                                                                 : Ptp.Slave

        RadioButton {
            id: masterRadioButton
            text: Strings.ptpModeToString(Ptp.Master)
            checked: ptpModel == null || ptpModel.ptpMode === Ptp.Master
        }

        RadioButton {
            text: Strings.ptpModeToString(Ptp.Slave)
            checked: ptpModel == null ? false
                                      : ptpModel.ptpMode === Ptp.Slave
            enabled: ptpModeLayout.portSlaveIndex == -1 ? true
                                                        : networkConfigurationIndex == ptpModeLayout.portSlaveIndex
        }

        Item { Layout.fillWidth: true; }

        Controls.ComboBox {
            Layout.alignment: Qt.AlignRight
            model: [ qsTr("Custom"), "8265.1", "8275.1", "8275.2" ]
            value: qsTr("Custom")
        }
    }

    Loader {
        width: parent.width
        active: ptpModeLayout.portSlaveIndex == -1 ? false
                                                   : networkConfigurationIndex != ptpModeLayout.portSlaveIndex
        visible: active
        sourceComponent: Label {
            wrapMode: Label.WordWrap
            text: qsTr("There can only be 1 slave on the current port")
        }
    }

    RowLayout {
        spacing: Theme.indent

        Grid {
            Layout.fillWidth: true
            columns: 2
            spacing: Theme.middleIndent

            verticalItemAlignment: Qt.AlignVCenter

            Label {
                text: qsTr("Clock ID")
                font.bold: true
            }

            TextField {
                id: clockIdField
                text: ptpModel == null ? ""
                                       : ptpModel.clockId
            }

            Label {
                text: qsTr("Remote master IPv4")
                font.bold: true
                visible: ptpModel != null && ptpModel.ptpMode === Ptp.Slave
            }

            TextField {
                id: remoteMasterAddressField
                text: ptpModel == null ? ""
                                       : ptpModel.remoteMasterAddress
                visible: ptpModel != null && ptpModel.ptpMode === Ptp.Slave
                validator: IpAddressValidator {}
            }

            Label {
                text: qsTr("Tx Mode")
                font.bold: true
            }

            Controls.ComboBox {
                id: txModeComboBox
                model: [ Ptp.Multicast, Ptp.Unicast, Ptp.Mixed ]
                delegateTextFunc: Strings.txModeToString
                value: ptpModel == null ? Ptp.Multicast
                                        : ptpModel.txMode
            }

            Label {
                text: qsTr("Way Mode")
                font.bold: true
            }

            Controls.ComboBox {
                id: wayModeComboBox
                model: [ Ptp.OneWay, Ptp.TwoWay ]
                delegateTextFunc: Strings.wayModeToString
                value: ptpModel == null ? Ptp.OneWay
                                        : ptpModel.wayMode
            }

            Label {
                text: qsTr("Sync Mode")
                font.bold: true
            }

            Controls.ComboBox {
                id: syncModeComboBox
                model: [ Ptp.OneStep, Ptp.TwoStep ]
                delegateTextFunc: Strings.syncModeToString
                value: ptpModel == null ? Ptp.OneStep
                                        : ptpModel.syncMode
            }

            Label {
                text: qsTr("Delay Mechanism")
                font.bold: true
            }

            Controls.ComboBox {
                id: delayMechanismComboBox
                model: [ Ptp.E2E, Ptp.P2P ]
                delegateTextFunc: Strings.delayMechanismToString
                value: ptpModel == null ? Ptp.E2E
                                        : ptpModel.delayMechanism
            }

            Label {
                text: qsTr("Domain Number")
                font.bold: true
            }

            TextField {
                id: domainNumberField
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
                text: ptpModel == null ? ""
                                       : ptpModel.domainNumber
            }

            Label {
                text: qsTr("Protocol")
                font.bold: true
            }

            Controls.ComboBox {
                id: protocolComboBox
                model: [ Ptp.L2MCast1, Ptp.L2MCast2, Ptp.Ipv4 ] //Ipv6
                delegateTextFunc: Strings.protocolToString
                value: ptpModel == null ? Ptp.L2MCast1
                                        : ptpModel.protocol
            }
        }


        Grid {
            Layout.fillWidth: true
            verticalItemAlignment: Qt.AlignVCenter

            columns: 2
            spacing: Theme.middleIndent

            Label {
                text: qsTr("Announce Timeout")
                font.bold: true
            }

            Controls.ComboBox {
                id: announceTimoutComboBox
                model: [ 128, 64, 32, 16, 8, 4, 2, 1, 0, -1, -2, -4, -8, -16, -32, -64, -128, ]
                value: ptpModel == null ? 0
                                        : ptpModel.announceTimeout
            }

            Label {
                text: qsTr("Sync interval")
                font.bold: true
            }

            Controls.ComboBox {
                id: syncIntervalComboBox
                model: [ 128, 64, 32, 16, 8, 4, 2, 1, 0, -1, -2, -4, -8, -16, -32, -64, -128, ]
                value: ptpModel == null ? 0
                                        : ptpModel.syncInterval
            }

            Label {
                text: qsTr("Delay Request Interval")
                font.bold: true
            }

            Controls.ComboBox {
                id: delayRequestComboBox
                model: [ 128, 64, 32, 16, 8, 4, 2, 1, 0, -1, -2, -4, -8, -16, -32, -64, -128, ]
                value: ptpModel == null ? 0
                                        : ptpModel.delayRequest
            }

            Label {
                text: qsTr("PDelay Request Interval")
                font.bold: true
            }

            Controls.ComboBox {
                id: pDelayRequestComboBox
                model: [ 128, 64, 32, 16, 8, 4, 2, 1, 0, -1, -2, -4, -8, -16, -32, -64, -128, ]
                value: ptpModel == null ? 0
                                        : ptpModel.pDelayRequest
            }

            // _________________________

            Label {
                text: qsTr("Priority 1")
                font.bold: true
            }

            TextField {
                id: pri1Field
                text: ptpModel == null ? ""
                                       : ptpModel.priority1
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
            }

            Label {
                text: qsTr("Priority 2")
                font.bold: true
            }

            TextField {
                id: pri2Field
                text: ptpModel == null ? ""
                                       : ptpModel.priority2
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
            }

            Label {
                text: qsTr("Clock Class")
                font.bold: true
            }

            TextField {
                id: clockClassField
                text: ptpModel == null ? ""
                                       : ptpModel.clockClass
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
            }
        }
    }

    Button {
        //        enabled: ntpModel == null ||
        //                 comboBox.currentIndex !== ntpModel.stratum ||
        //                 serverRefIdField.text !== ntpModel.serverRefId
        anchors.right: parent.right
        text: qsTr("Apply")
        onClicked: {
            device.send(card.ptpConfigurationCommand(portModel, networkConfigurationIndex,
                                                     txModeComboBox.value, wayModeComboBox.value, syncModeComboBox.value,
                                                     delayMechanismComboBox.value, domainNumberField.text, protocolComboBox.value,
                                                     ptpModeLayout.ptpMode, clockIdField.text,
                                                     announceTimoutComboBox.value, syncIntervalComboBox.value, delayRequestComboBox.value,
                                                     pDelayRequestComboBox.value, pri1Field.text, pri2Field.text, clockClassField.text,
                                                     remoteMasterAddressField.text))
            controlPopup.close()
        }
    }
}

