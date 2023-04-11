import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import SSU 1.0
import Theme 1.0
import Strings 1.0

import Models 1.0

import "gnss/"
import "private/"
import "timeprotocol/"

CardPage {
    id: page
    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    footerData: [
        TabBar {
            id: bar
            currentIndex: 0//3

            TabButton { text: qsTr("Home"); width: implicitWidth; }
            TabButton { text: qsTr("GNSS"); width: implicitWidth; }
            TabButton { text: qsTr("Satellite Data"); width: implicitWidth; }
            TabButton { text: qsTr("Port 0"); width: implicitWidth; }
            TabButton { text: qsTr("Port 1"); width: implicitWidth; }
        }
    ]

    header: Pane {
        spacing: 0
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent;

        contentItem: RowLayout {
            id: headerContent
            spacing: 0

            property bool shortContent: false

            Controls.Button {
                id: referenceGeneratorButton
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                visible: card.referenceGeneratorAddress !== -1
                symbol: SSU.addressToString(card.referenceGeneratorAddress)
                size: Theme.pixelSize * 0.9

                textLabel.visible: !headerContent.shortContent

                text: qsTr("Reference generator")
                clickable: false

                backgroundColor: Material.color(Material.LightGreen)
                toolTip.text: card === null ? ""
                                            : qsTr("Reference generator") + ": " + referenceGeneratorButton.symbol
            }

            Controls.Button {
                id: activeChannelButton
                Layout.alignment: Qt.AlignVCenter
                visible: card.activeChannel !== 255 && card.pllState !== SSU.PllState.Warmup

                symbol: device == null ? ""
                                       : device.channelDescription.cardInputChannelToString(card.type, card.activeChannel, ChannelDescription.Short)

                size: Theme.pixelSize * 0.9

                textLabel.visible: !headerContent.shortContent

                text: qsTr("Channel")
                clickable: false

                backgroundColor: Material.color(Material.LightGreen)

                toolTip.visible: activeChannelButton.hovered && card.activeChannel !== 255

                toolTip.text: card === null || device === null ? ""
                                                               : qsTr("Active channel") + ": " + device.channelDescription.cardInputChannelToString(card.type, card.activeChannel)
            }

            Controls.Button {
                id: workingModeButton
                Layout.alignment: Qt.AlignVCenter
                text: qsTr("Mode")
                clickable: page.controlRights

                textLabel.visible: !headerContent.shortContent

                symbol: {
                    if(card === null) return ""
                    switch(card.mode) {
                    case SSU.WorkingMode.AutoMode:          return "A"
                    case SSU.WorkingMode.ManualMode:        return "M"
                    default: return ""
                    }
                }

                toolTip.text: qsTr("Mode") + ": " + SSU.workingModeToString(card === null ? 0 : card.mode)

                Controls.ToolTip {
                    x: workingModeButton.toolTip.x; y: workingModeButton.toolTip.y + workingModeButton.toolTip.height + Theme.smallIndent
                    visible: workingModeButton.hovered && workingModeButton.clickable
                    delay: 600
                    text: qsTr("Click to set mode")
                }

                onClicked: controlPopup.openPopup(workingModeSettingsComponent, -1, qsTr("GNSS mode"))
            }

            Controls.Button {
                id: ssmButton
                Layout.alignment: Qt.AlignVCenter
                textLabel.visible: !headerContent.shortContent
                text: qsTr("SSM")
                symbol: SSU.ssmToString(card.ssm)
                toolTip.text: qsTr("SSM") + ": " + symbol
                clickable: false

                //                Controls.ToolTip {
                //                    x: ssmButton.toolTip.x; y: ssmButton.oolTip.y + ssmButton.toolTip.height + Theme.smallIndent
                //                    visible: ssmButton.hovered && ssmButton.clickable
                //                    delay: 600
                //                    text: qsTr("Click to set ssm")
                //                }
                //                onClicked: controlPopup.openPopup(ssmSettingsComponent, -1, qsTr("SSM"))
            }

            Item {
                Layout.fillWidth: true

                function calcWidth() {
                    if(width <= 0)
                        headerContent.shortContent = true
                    else {
                        var butListWidth = (workingModeButton.visible ? workingModeButton.textLabel.width
                                                                      : 0) +
                                (referenceGeneratorButton.visible ? referenceGeneratorButton.textLabel.width
                                                                  : 0) +
                                (activeChannelButton.visible ? activeChannelButton.textLabel.width
                                                             : 0) +
                                (ssmButton.visible ? ssmButton.textLabel.width
                                                   : 0)
                        if(width > butListWidth * 1.2)
                            headerContent.shortContent = false
                    }
                }

                Component.onCompleted: calcWidth()
                onWidthChanged: calcWidth()
            }

            PllStateItem {
                id: pllStateButton
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                state: card === null ? 0 : card.pllState
                title: card === null || card.pllStateTimer === 0 ? ""
                                                                 : Strings.minToDateString(card.pllStateTimer)

                readonly property bool toolTipVisible: card !== null && card.mode === SSU.WorkingMode.AutoMode &&
                                                       (card.pllState === SSU.PllState.Freerun || card.pllState === SSU.PllState.Holdover)

                toolTip.timeout: 10000
                toolTip.visible: pllStateButton.hovered && pllStateButton.title.length > 0
                toolTip.contentItem: Label {    // Чтобы сделать перенос у текста..
                    wrapMode: Label.WordWrap
                    text: pllStateButton.toolTip.text
                }

                toolTip.text: {
                    //                    if(pllStateButton.state === SSU.PllState.Tracking)
                    //                        return qsTr("Ability to switch to forced hold mode through") + " " + pllStateButton.title
                    //                    else {
                    var str = qsTr("Switch to mode") + " <b>"

                    switch(pllStateButton.state) {
                    case SSU.PllState.Warmup:
                        str += SSU.pllStateToString(SSU.PllState.Adjustment)
                        break
                    case SSU.PllState.Adjustment:
                        str += SSU.pllStateToString(SSU.PllState.Tracking)
                        break
                    case SSU.PllState.Holdover:
                        str += SSU.pllStateToString(SSU.PllState.Freerun)
                        break
                    default:
                        return ""
                    }
                    return str + "</b> " + qsTr("through") + " " + pllStateButton.title
                    //                    }
                }
            }   // PllStateItem

        }   // contentItem
    }   // Header

    Controls.LoaderStackLayout {
        width: parent.width; height: parent.height
        currentIndex: bar.currentIndex

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: GnssCardChannelsPage {}
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: GnssModulePage {}
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: SatellitesDataForm {
                model: card == null ? null
                                    : card.satelliteList
            }
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: EthernetPortPage {
                portModel: card == null ? null
                                        : card.port0
            }
        }

        Loader {
            Layout.fillWidth: true; Layout.fillHeight: true
            active: false
            sourceComponent: EthernetPortPage {
                portModel: card == null ? null
                                        : card.port1
            }
        }
    }


    Dialogs.SettingsPopup {
        id: controlPopup

        function openPopup(component, numberOfChannel, title, fillParentWidth = false) {
            controlPopup.title = title
            controlPopup.fillParentWidth = fillParentWidth
            loader.numberOfChannel = numberOfChannel
            loader.sourceComponent = component
            open()
        }

        onClosed: loader.sourceComponent = null

        contentItem: Loader {
            id: loader
            property int numberOfChannel: -1
            sourceComponent: null
        }
    }

    Component {
        id: ssmSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.ssm
                model: [
                    SSU.SSM.None,
                    SSU.SSM.PRC,
                    SSU.SSM.SSU_T,
                    SSU.SSM.SSU_L,
                    SSU.SSM.SEC,
                    SSU.SSM.DNU
                ]

                delegateTextFunc: SSU.ssmToString
            }

            Button {
                anchors.right: parent.right
                enabled: comboBox.value !== card.ssm
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetSsm(comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: workingModeSettingsComponent

        Column {
            spacing: 0

            readonly property int newWorkingMode: autoMode.checked ? SSU.WorkingMode.AutoMode :
                                                                     SSU.WorkingMode.ManualMode

            RadioButton {
                id: autoMode
                width: parent.width
                //                topInset: 0; bottomInset: 0
                checked: card.mode ===  SSU.WorkingMode.AutoMode
                text: SSU.workingModeToString(SSU.WorkingMode.AutoMode)
            }

            RadioButton {
                id: manualButton
                width: parent.width
                //                topInset: 0; bottomInset: 0
                checked: card.mode === SSU.WorkingMode.ManualMode
                text: SSU.workingModeToString(SSU.WorkingMode.ManualMode)
            }

            Controls.ComboBox {
                id: manualChannelComboBox
                topInset: 0; bottomInset: 0
                width: parent.width
                visible: manualButton.checked
                model: device.options.kNumberOfGnssCardInChannels
                value: card.manualChannel
                delegateTextFunc: function(index) { return device == null ? ""
                                                                          : device.channelDescription.cardInputChannelToString(card.type, index) }
            }

            Button {
                anchors.right: parent.right
                enabled: card.mode !== newWorkingMode || card.manualChannel !== manualChannelComboBox.value
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetWorkingMode(newWorkingMode, manualChannelComboBox.value))
                    controlPopup.close()
                }
            }
        }
    }   // component

}





