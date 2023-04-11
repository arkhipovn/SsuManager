import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import QCC.Validators 1.0

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import Theme 1.0
import SSU 1.0

import Strings 1.0
import Models.Port 1.0

Item {
    id: page
    property var portModel: null

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    RowLayout {
        width: parent.width; height: parent.height
        spacing: Theme.indent

        GridLayout {
            Layout.fillWidth: true; Layout.fillHeight: true

            rowSpacing: Theme.middleIndent
            columnSpacing: Theme.middleIndent

            Column {
                Layout.column: 0; Layout.row: 0
                Layout.alignment: Qt.AlignTop

                Controls.Group {
                    title: qsTr("1PPS & ToD")

                    onClicked: if(page.controlRights) controlPopup.openPopup(ppsSourceSettingsComponent, -1, title)

                    contentItem: Grid {
                        columns: 2
                        spacing: Theme.middleIndent

                        Label {
                            text: qsTr("Source")
                            font.bold: true
                        }

                        Label {
                            text: portModel == null ? "-"
                                                    : Strings.ppsSourceToString(portModel.ppsSource)
                        }

                        //                    Label {
                        //                        text: qsTr("Status")
                        //                        font.bold: true
                        //                    }

                        //                    Label {
                        //                        color: portModel == null ? Material.foreground
                        //                                                 : portModel.ptpClockStatus === EthernetPort.Lock ? Material.color(Material.Green)
                        //                                                                                                  : Material.color(Material.Red)
                        //                        text: portModel == null ? "-"
                        //                                                : Strings.ptpClockStatusToString(portModel.ptpClockStatus)
                        //                    }
                    }
                }

                QCC.LinkButton {
                    text: qsTr("Statistics")
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                    color: Material.accentColor
                    onClicked: controlPopup.openPopup(portStatisticsComponent, -1, qsTr("Statistics"), true)
                }
            }


            Controls.Group {
                Layout.column: 1; Layout.row: 0
                Layout.alignment: Qt.AlignTop
                Layout.rowSpan: 2

                title: qsTr("Port")

                onClicked: if(page.controlRights) controlPopup.openPopup(portSettingsComponent, -1, title)

                contentItem: Grid {
                    columns: 2
                    spacing: Theme.middleIndent

                    Label {
                        text: qsTr("MAC address")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null || portModel.macAddress.length === 0 ? "-"
                                                                                     : portModel.macAddress
                    }

                    Label {
                        text: qsTr("Speed")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null ? "-"
                                                : Strings.portSpeedToString(portModel.speed)
                    }

                    Label {
                        text: qsTr("MTU")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null ? "-"
                                                : portModel.mtu + " " + qsTr("bytes")
                    }

                    Label {
                        text: qsTr("Swap")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null || !portModel.swap ? qsTr("Disabled")
                                                                   : qsTr("Enabled")
                    }

                    Label {
                        text: qsTr("Loopback")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null || !portModel.loopback ? qsTr("Disabled")
                                                                       : qsTr("Enabled")
                    }
                }
            }   // Group

            Controls.Group {
                Layout.column: 2; Layout.row: 0
                Layout.alignment: Qt.AlignTop
                Layout.rowSpan: 2

                title: qsTr("SyncE Configuration")

                onClicked: if(page.controlRights) controlPopup.openPopup(syncEConfigurationComponent, -1, title)


                contentItem: Grid {
                    columns: 2
                    spacing: Theme.middleIndent

                    Label {
                        text: qsTr("SyncE Mode")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null || !portModel.syncEMode ? qsTr("Disabled")
                                                                        : qsTr("Enabled")
                    }

                    Label {
                        text: qsTr("QL Mode")
                        font.bold: true
                    }

                    Label {
                        text: portModel == null || !portModel.qlMode ? qsTr("Disabled")
                                                                     : qsTr("Enabled")
                    }

                    Label {
                        text: qsTr("Assumed QL")
                        font.bold: true
                    }

                    Label {
                        text: SSU.ssmToString(portModel == null ? SSU.None
                                                                : portModel.assumedQl)                  // PRC/SSU-A/SSU-B/EEC1/None
                    }
                }   // Column
            }   // Group

            Controls.GroupBox {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.column: 0; Layout.row: 2
                Layout.columnSpan: 3

                label: Label {
                    width: parent.width
                    text: qsTr("Network configurations")
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                    color: Material.accentColor
                    font.bold: true
                }

                contentItem: NetworkConfigurationList {
                    id: networkConfigurationList
                    model: portModel == null ? null
                                             : portModel.networkConfigurationList
                }
            }
        }   // GridLayout

        Loader {
            id: portConfigLoader
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.maximumWidth: Theme.indent * 30
            Layout.alignment: Qt.AlignLeft
            clip: true

            active: networkConfigurationList.currentNetworkConfiguration !== null
            sourceComponent: networkConfigurationList.currentNetworkConfiguration === null ? null
                                                                                           : networkConfigurationList.currentNetworkConfiguration.isValid ? configuredConfigurationComponent
                                                                                                                                                          : emptyConfigurationComponent
        }

        //        Item { Layout.fillWidth: true; }
    }   // RowLayout

    Component {
        id: configuredConfigurationComponent

        Flickable {
            width: portConfigLoader.width
            contentHeight: column.height
            interactive: height < contentHeight

            ColumnLayout {
                id: column
                width: parent.width
                spacing: Theme.indent

                RowLayout {
                    Layout.fillWidth: true

                    Controls.Group {
                        title: qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1)

                        onClicked: if(page.controlRights) controlPopup.openPopup(enableConfigurationComponent, networkConfigurationList.currentIndex,
                                                                                 qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1))

                        contentItem: Grid {
                            columns: 2
                            spacing: Theme.middleIndent

                            Label {
                                text: qsTr("Ip4")
                                font.bold: true
                            }

                            Label {
                                text: networkConfigurationList.currentNetworkConfiguration.ip4.length === 0 ? "-"
                                                                                                            : networkConfigurationList.currentNetworkConfiguration.ip4
                            }

                            Label {
                                text: qsTr("Mask")
                                font.bold: true
                            }

                            Label {
                                text: networkConfigurationList.currentNetworkConfiguration.mask.length === 0 ? "-"
                                                                                                             : networkConfigurationList.currentNetworkConfiguration.mask
                            }

                            Label {
                                text: qsTr("Gateway")
                                font.bold: true
                            }

                            Label {
                                text: networkConfigurationList.currentNetworkConfiguration.gateway.length === 0 ? "-"
                                                                                                                : networkConfigurationList.currentNetworkConfiguration.gateway
                            }
                        }
                    }

                    Item { Layout.fillWidth: true; }

                    Column {
                        Layout.alignment: Qt.AlignTop

                        QCC.LinkButton {
                            text: qsTr("Update")
                            color: Material.accentColor
                            onClicked: device.send(card.updateNetworkConfigurationCommand(portModel, networkConfigurationList.currentIndex))
                        }

                        Loader {
                            active: page.controlRights
                            visible: active

                            sourceComponent: QCC.LinkButton {
                                text: qsTr("Remove")
                                color: Material.color(Material.Red)

                                onClicked: removeConfPopup.open()

                                Dialogs.ConfirmationPopup {
                                    id: removeConfPopup
                                    text: qsTr("Are you sure you want to delete the configuration?")
                                    onClicked: device.send(card.clearConfigurationCommand(portModel, networkConfigurationList.currentIndex))
                                }
                            }
                        }
                    }
                }

                Loader {
                    active: !networkConfigurationList.currentNetworkConfiguration.ntp.isValid ||
                            !networkConfigurationList.currentNetworkConfiguration.ptp.isValid
                    visible: active
                    sourceComponent: Row {
                        spacing: Theme.middleIndent

                        Button {
                            visible: !networkConfigurationList.currentNetworkConfiguration.ntp.isValid
                            text: qsTr("Add NTP")
                            onClicked: controlPopup.openPopup(ntpConfigurationComponent, -1, qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1) +
                                                              ". " + qsTr("NTP"))
                        }

                        Button {
                            visible: !networkConfigurationList.currentNetworkConfiguration.ptp.isValid
                            text: qsTr("Add PTP")
                            onClicked: controlPopup.openPopup(ptpConfigurationComponent, -1, qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1) +
                                                              ". " + qsTr("PTP"), true)
                        }
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    active: networkConfigurationList.currentNetworkConfiguration.ntp.isValid
                    visible: active
                    sourceComponent: NtpPage {
                        networkConfigurationIndex: networkConfigurationList.currentIndex
                        ntpModel: networkConfigurationList.currentNetworkConfiguration.ntp
                    }
                }

                Loader {
                    Layout.fillWidth: true
                    active: networkConfigurationList.currentNetworkConfiguration.ptp.isValid
                    visible: active
                    sourceComponent: PtpPage {
                        networkConfigurationIndex: networkConfigurationList.currentIndex
                        ptpModel: networkConfigurationList.currentNetworkConfiguration.ptp
                    }
                }
            }   // ColumnLayout
        }
    }

    Component {
        id: ntpConfigurationComponent
        NtpConfiguration {
            networkConfigurationIndex: networkConfigurationList.currentIndex
        }
    }

    Component {
        id: ptpConfigurationComponent
        PtpConfiguration {
            networkConfigurationIndex: networkConfigurationList.currentIndex
        }
    }

    Component {
        id: emptyConfigurationComponent

        RowLayout {
            Column {
                Layout.alignment: Qt.AlignTop

                topPadding: Theme.smallIndent
                leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                spacing: Theme.smallIndent

                Label {
                    text: qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1)
                    color: Material.accentColor
                    font.bold: true
                    bottomPadding: Theme.smallIndent
                }

                Label {
                    text: qsTr("Configuration not configured")
                }

                Loader {
                    active: page.controlRights
                    visible: active
                    sourceComponent: Button {
                        text: qsTr("Configure")
                        onClicked: controlPopup.openPopup(enableConfigurationComponent, networkConfigurationList.currentIndex, qsTr("Network Configuration") + " " + (networkConfigurationList.currentIndex + 1))
                    }
                }
            }

            Item { Layout.fillWidth: true; }

            QCC.LinkButton {
                Layout.alignment: Qt.AlignTop
                text: qsTr("Update")
                color: Material.accentColor
                onClicked: device.send(card.updateNetworkConfigurationCommand(portModel, networkConfigurationList.currentIndex))
            }
        }
    }

    Component {
        id: enableConfigurationComponent

        Column {
            spacing: 0

            QCC.TextField {
                id: ipField
                width: parent.width
                placeholderText: qsTr("IP address")
                text: networkConfigurationList.currentNetworkConfiguration.ip4
                validator: IpAddressValidator {}
            }

            QCC.TextField {
                id: ipMaskField
                width: parent.width
                placeholderText: qsTr("IP mask")
                text: networkConfigurationList.currentNetworkConfiguration.mask
                validator: IpAddressValidator {}
            }

            QCC.TextField {
                id: gatewayField
                width: parent.width
                placeholderText: qsTr("Gateway")
                text: networkConfigurationList.currentNetworkConfiguration.gateway
                validator: IpAddressValidator {}
            }

            Button {
                anchors.right: parent.right;
                enabled: (ipField.length > 0 && ipField.acceptableInput) &&
                         (ipMaskField.length > 0 && ipMaskField.acceptableInput) &&
                         (gatewayField.length > 0 && gatewayField.acceptableInput)
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.networkConfigurationCommand(portModel, numberOfChannel,
                                                                 ipField.text, ipMaskField.text, gatewayField.text))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: portStatisticsComponent

        PortStatistics {
            model: portModel == null ? null
                                     : portModel.statistics
        }
    }

    Component {
        id: ppsSourceSettingsComponent

        Column {
            spacing: Theme.smallIndent

            readonly property bool confConfigured: {
                for(var i = 0; i < portModel.networkConfigurationList.length; ++i) {
                    if(portModel.networkConfigurationList[i].isValid)
                        return true
                }
                return false
            }

            Controls.ComboBox {
                id: comboBox
                enabled: !confConfigured
                width: parent.width
                model: [ EthernetPort.MasterInternal, EthernetPort.MasterGnss,
                    EthernetPort.MasterSlave, EthernetPort.Slave ]
                delegateTextFunc: Strings.ppsSourceToString
                value: portModel == null ? ""
                                         : portModel.ppsSource
            }

            Label {
                visible: confConfigured
                color: Material.color(Material.Orange)
                text: qsTr("To change 1PPS and ToD source delete all configurations")   // Чтобы изменить источник 1PPS и ToD удалите все конфигурации
            }

            Button {
                anchors.right: parent.right;
                enabled: confConfigured ? true
                                        : comboBox.value !== portModel.ppsSource
                text: confConfigured ? qsTr("Delete all configurations")
                                     : qsTr("Apply")
                onClicked: {
                    if(confConfigured) {
                        for(var i = 0; i < portModel.networkConfigurationList.length; ++i) {
                            if(portModel.networkConfigurationList[i].isValid) {
                                device.send(card.clearConfigurationCommand(portModel, i))
                            }
                        }
                    }
                    else {
                        device.send(card.commandToSetPpsSource(portModel, comboBox.value))
                    }

                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: portSettingsComponent

        Column {
            spacing: 0

            QCC.TextField {
                id: macAddressField
                width: parent.width
                placeholderText: qsTr("MAC address")
                text: portModel == null ? 0
                                        : portModel.macAddress
                validator: MacAddressValidator {}
            }

            Label {
                text: qsTr("Speed")
            }

            Controls.ComboBox {
                id: speedComboBox
                width: parent.width
                model: [ EthernetPort.Auto, EthernetPort.M1000, EthernetPort.M100, EthernetPort.M10 ]
                delegateTextFunc: Strings.portSpeedToString
                value: portModel == null ? ""
                                         : portModel.speed === EthernetPort.Invalid || portModel.speed === EthernetPort.NoCapture ? EthernetPort.Auto
                                                                                                                                  : portModel.speed
            }

            QCC.TextField {
                id: mtuField
                width: parent.width
                placeholderText: qsTr("MTU")
                text: portModel == null ? 0
                                        : portModel.mtu
                validator: IntValidator {
                    bottom: 0
                    top: 9600
                }
            }

            CheckBox {
                id: swapCheckBox
                width: parent.width
                text: qsTr("Swap")
                checked: portModel == null ? false
                                           : portModel.swap
            }

            CheckBox {
                id: loopbackCheckBox
                width: parent.width
                text: qsTr("Loopback")
                checked: portModel == null ? false
                                           : portModel.loopback
            }

            Button {
                enabled: portModel == null ||
                         macAddressField.text !== portModel.macAddress ||
                         speedComboBox.value !== portModel.speed ||
                         mtuField.text != portModel.mtu ||
                         swapCheckBox.checked !== portModel.swap ||
                         loopbackCheckBox.checked !== portModel.loopback
                anchors.right: parent.right;
                text: qsTr("Apply")
                onClicked: {
                    if(macAddressField.text !== portModel.macAddress && macAddressField.acceptableInput)
                        device.send(card.commandToSetPortMacAddress(portModel, macAddressField.text))

                    if(speedComboBox.value !== portModel.speed)
                        device.send(card.commandToSetPortSpeed(portModel, speedComboBox.value))

                    if(mtuField.text !== portModel.mtu && mtuField.acceptableInput)
                        device.send(card.commandToSetPortMtu(portModel, mtuField.text))

                    if(swapCheckBox.checked !== portModel.swap)
                        device.send(card.commandToSetPortSwap(portModel, swapCheckBox.checked))

                    if(loopbackCheckBox.checked !== portModel.loopback)
                        device.send(card.commandToSetPortLoopback(portModel, loopbackCheckBox.checked))

                    controlPopup.close()
                }
            }
        }
    }


    Component {
        id: syncEConfigurationComponent

        Column {
            spacing: 0

            CheckBox {
                id: syncEModeCheckBox
                width: parent.width
                text: qsTr("SyncE Mode")
                checked: portModel == null ? false
                                           : portModel.syncEMode
            }

            CheckBox {
                enabled: syncEModeCheckBox.checked
                width: parent.width
                text:qsTr("QL Mode")
                checked: portModel == null ? false
                                           : portModel.qlMode
            }

            //            ComboBox {
            //                enabled: syncEModeCheckBox.checked
            //                width: parent.width
            //                text: SSU.ssmToString(portModel == null ? SSU.None
            //                                                        : portModel.assumedQl)                  // PRC/SSU-A/SSU-B/EEC1/None
            //            }

            Button {
                anchors.right: parent.right;
                text: qsTr("Apply")
                onClicked: {
                    controlPopup.close()
                }
            }
        }
    }

}

