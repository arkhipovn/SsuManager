import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC
import QCC.Controls 2.0 as QCC2

import SSU 1.0
import Theme 1.0

import Models 1.0

import "private/"
import "private/InputChannelState.js" as InputChannelState

CardPage {
    id: page
    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    header: Pane {
        spacing: 0
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent;

        contentItem: RowLayout {
            Controls.Button {
                id: channelSettingsButton
                topInset: 0; bottomInset: 0
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                symbol: "\uf140"
                text: qsTr("Channel settings")
                highlighted: channelSettingsDialogLoader.active
                onClicked: channelSettingsDialogLoader.active = true
            }

            Label {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                text: qsTr("Group") + " " + (groupNumber + 1)
                font.bold: true
                color: Material.accentColor
            }
        }
    }

    footerData: [
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: card.isMaster ? SSU.controlModeToString(SSU.Master)
                                : SSU.controlModeToString(SSU.Slave)
            clickable: false
            toolTip.text: card.isMaster ? SSU.controlModeToString(SSU.Master)
                                        : SSU.controlModeToString(SSU.Slave) +
                                          ". " + qsTr("Reads parameters from Master")
        }
    ]

    Controls.GroupBox {
        id: paramGroupBox
        width: parent.width; height: parent.height

        label: Controls.TableHeader {
            width: paramGroupBox.width
            leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
            spacing: Theme.smallIndent
            implicitHeight: implicitContentHeight

            Item { Layout.preferredWidth: Theme.textFieldWidth; }

            Controls.TableCell {
                Layout.preferredWidth: 2
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Description")
            }

            Controls.TableCell {
                Layout.preferredWidth: 2
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Signal type")
            }

            Controls.TableCell {
                Layout.preferredWidth: 3
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Threshold status")
            }

            Controls.TableCell {
                Layout.preferredWidth: 1
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("SSM")
            }

            Controls.TableCell {
                Layout.preferredWidth: 1
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Wait to restore time")
            }
        }   // Controls.RowLayout signal

        ListView {
            clip: true
            width: parent.width; height: parent.height
            spacing: Theme.smallIndent
            model: card.inputChannels

            boundsBehavior: ListView.StopAtBounds
            flickableDirection: ListView.VerticalFlick
            headerPositioning: ListView.OverlayHeader

            delegate: inputChannelDelegate
        }
    }

    Component {
        id: inputChannelDelegate

        ChannelPane {
            id: channelPane
            width: ListView.view.width

            readonly property int numberOfChannel: index
            readonly property var channel: modelData

            ChannelStatusLabel {
                id: channelStateButton
                Layout.preferredWidth: Theme.textFieldWidth
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                text: device == null ? ""
                                     : device.channelDescription.cardInputChannelToString(card.type, channelPane.numberOfChannel, ChannelDescription.Short)
                backgroundColor: InputChannelState.stateColor(card.leds.data[numberOfChannel + 1])

                toolTip.text: {
                    if(device == null)
                        return ""
                    var name = device.channelDescription.cardInputChannelToString(card.type, channelPane.numberOfChannel)
                    return name + " - " + InputChannelState.stateToolTip(channel)
                }
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 2
                horizontalAlignment: Label.AlignLeft

                text: {
                    if(device == null)
                        return ""
                    device.channelDescription.hasTag
                    return device.channelDescription.ssuInputChannelTag(groupNumber * 4 + channelPane.numberOfChannel)
                }

                clickable: true
                toolTip.text: qsTr("Click to set channel description")
                onClicked: controlPopup.openPopup(descriptionSettingsComponent, groupNumber * 4 + channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("Channel description"))
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 2
                horizontalAlignment: Label.AlignLeft
                text: SSU.signalTypeToString(channel.type)
                clickable: page.controlRights
                toolTip.text: qsTr("Click to set signal type")
                onClicked: controlPopup.openPopup(signalTypeSettingsComponent, channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("Signal type"))
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 3
                horizontalAlignment: Label.AlignLeft
                text: SSU.statusThresholdToString(channel.statusThreshold)
                clickable: page.controlRights
                toolTip.text: qsTr("Click to set status threshold")
                onClicked: controlPopup.openPopup(signalStatusThresholdSettingsComponent, channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("Threshold status"))
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 1
                horizontalAlignment: Label.AlignLeft
                text: SSU.ssmToString(channel.ssm)
                clickable: device == null ? false
                                          : channel.type === SSU.SignalType.E1 || channel.type === SSU.SignalType.Off ? false
                                                                                                                      : page.controlRights
                toolTip.text: qsTr("Click to set SSM")
                onClicked: controlPopup.openPopup(signalSsmSettingsComponent, channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("SSM"))
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 1
                horizontalAlignment: Label.AlignLeft
                text: channel.waitToRestoreTime + " " + qsTr("sec")
                clickable: page.controlRights
                toolTip.text: qsTr("Click to set the waiting time for signal recovery")
                onClicked: controlPopup.openPopup(signalRestoreTimeSettingsComponent, channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("Restore time"))
            }
        }
    }   // deviceInputDelegate



    // Settings !!

    Dialogs.SettingsPopup {
        id: controlPopup

        function openPopup(component, numberOfChannel, title) {
            controlPopup.title = title
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
        id: descriptionSettingsComponent

        Column {
            TextField {
                id: textField
                width: parent.width
                focus: true
                selectByMouse: true
                wrapMode: TextField.Wrap

                text: {
                    if(device == null)
                        return ""
                    device.channelDescription.hasTag
                    return device.channelDescription.ssuInputChannelTag(numberOfChannel)
                }
            }

            Button {
                anchors.right: parent.right;
                text: qsTr("Apply")
                onClicked: {
                    device.setTag(numberOfChannel, textField.text)
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: signalTypeSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.inputChannels[numberOfChannel].type
                model: [
                    SSU.SignalType.Off,
                    SSU.SignalType.Auto,
                    SSU.SignalType.MHz1,
                    SSU.SignalType.KHz2048,
                    SSU.SignalType.MHz5,
                    SSU.SignalType.MHz10,
                    SSU.SignalType.E1
                ]
                delegateTextFunc: SSU.signalTypeToString
            }

            RowLayout {
                width: parent.width
                spacing: Theme.smallIndent

                Item { Layout.fillWidth: true; }    // Чтобы занять свободное пространство

                Button {
                    id: autoSelectionButton
                    text: qsTr("Auto")
                    visible: card.inputChannels[numberOfChannel].type !== SSU.SignalType.Auto

                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth

                    onClicked: {
                        device.send(card.commandForAutodetectingChannelType(numberOfChannel))
                        controlPopup.close()
                    }

                    Controls.ToolTip {
                        visible: autoSelectionButton.hovered
                        text: qsTr("Auto detection of signal type")
                    }
                }

                Button {
                    id: offChannelButton
                    visible: card.inputChannels[numberOfChannel].type !== SSU.SignalType.Off
                    text: qsTr("Switch off")
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth

                    onClicked: {
                        device.send(card.commandToSetTypeChannel(numberOfChannel, SSU.SignalType.Off))
                        controlPopup.close()
                    }
                }

                Button {
                    id: applyButton
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    enabled: comboBox.value !== card.inputChannels[numberOfChannel].type
                    text: qsTr("Apply")
                    onClicked: {
                        device.send(card.commandToSetTypeChannel(numberOfChannel, comboBox.value))
                        controlPopup.close()
                    }

                    Controls.ToolTip {
                        visible: applyButton.hovered
                        text: qsTr("Apply")
                    }
                }
            }
        }
    }

    Component {
        id: signalStatusThresholdSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.inputChannels[numberOfChannel].statusThreshold
                model: [
                    SSU.StatusThreshold.None,
                    SSU.StatusThreshold.Low,
                    SSU.StatusThreshold.High
                ]
                delegateTextFunc: SSU.statusThresholdToString
            }

            Button {
                anchors.right: parent.right
                enabled: comboBox.value !== card.inputChannels[numberOfChannel].statusThreshold
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetThresholdStatus(numberOfChannel, comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: signalSsmSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.inputChannels[numberOfChannel].ssm
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
                enabled: comboBox.value !== card.inputChannels[numberOfChannel].ssm
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetSsm(numberOfChannel, comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: signalRestoreTimeSettingsComponent

        Column {
            TextField {
                id: textField
                width: parent.width
                focus: true
                selectByMouse: true
                wrapMode: TextArea.Wrap
                text: card.inputChannels[numberOfChannel].waitToRestoreTime

                validator: IntValidator {
                    bottom: 0
                    top: 1275
                }
            }

            Label {
                width: parent.width
                wrapMode: Label.WordWrap
                visible: !textField.acceptableInput
                text: qsTr("Enter a value between 0 and 1275")
                color: Material.color(Material.Red)
            }

            Row {
                anchors.right: parent.right
                spacing: Theme.middleIndent

                Button {
                    visible: card.inputChannels[numberOfChannel].state & Vch003.RecoveryDelay
                    text: qsTr("Reset timeout") // Сброс времени ожидания
                    onClicked: device.send(card.commandToSetResetRestoreTimeout(numberOfChannel))
                }

                Button {
                    enabled: textField.text != card.inputChannels[numberOfChannel].waitToRestoreTime && textField.acceptableInput
                    text: qsTr("Apply")
                    onClicked: {
                        device.send(card.commandToSetRestoreTime(numberOfChannel, textField.text))
                        controlPopup.close()
                    }
                }
            }
        }
    }

    Loader {
        id: channelSettingsDialogLoader
        active: false
        sourceComponent: Dialogs.ModalDialog {
            id: channelSettingsDialog
            title: qsTr("Channel settings")
            width: Math.min(Theme.pixelSize * 30, 500); height: Math.min(55 * Theme.pixelSize, 1000)
            onClosing: channelSettingsDialogLoader.active = false

            header: Item {
                height: channelComboBox.height
                Controls.ComboBox {
                    id: channelComboBox
                    anchors { left: parent.left; right: parent.right; margins: Theme.indent; }
                    value: 0
                    model: card.inputChannels.length
                    delegateTextFunc: function(index) { return qsTr("Input") + " " + (index + 1) }

                    readonly property var channel: card.inputChannels[currentIndex]
                }
            }

            footer: Controls.ToolBar {
                Material.elevation: 0
                visible: page.controlRights
                leftPadding: Theme.indent; rightPadding: Theme.indent

                RowLayout {
                    width: parent.width
                    spacing: Theme.middleIndent

                    Item { Layout.fillWidth: true; }    // Чтобы занять свободное пространство

                    Button {
                        Layout.fillWidth: true
                        Layout.maximumWidth: implicitWidth
                        visible: channelComboBox.channel.state & Vch003.RecoveryDelay
                        text: qsTr("Reset timeout") // Сброс времени ожидания
                        onClicked: device.send(card.commandToSetResetRestoreTimeout(channelComboBox.currentIndex))
                    }

                    Button {
                        Layout.fillWidth: true
                        Layout.maximumWidth: implicitWidth
                        readonly property bool sensitivityChanged: failureTransmEndSensitivitySlider.currentValue !== channelComboBox.channel.failureTransmEndSensitivity ||
                                                                   crcErrorsSensitivitySlider.currentValue !== channelComboBox.channel.crcErrorsSensitivity ||
                                                                   lossOfMultiFrameSensitivitySlider.currentValue !== channelComboBox.channel.lossOfMultiFrameSensitivity ||
                                                                   lossOfFrameSensitivitySlider.currentValue !== channelComboBox.channel.lossOfFrameSensitivity ||
                                                                   hdb3ErrorSensitivitySlider.currentValue !== channelComboBox.channel.hdb3ErrorSensitivity

                        enabled: e1bitComboBox.value !== channelComboBox.channel.e1bit ||
                                 measurementModeCheckBox.checked !== channelComboBox.channel.measurementMode ||
                                 sensitivityChanged

                        text: qsTr("Apply")
                        onClicked: {
                            if(measurementModeCheckBox.checked !== channelComboBox.channel.measurementMode)
                                device.send(card.commandToSetMeasurementMode(channelComboBox.currentIndex, measurementModeCheckBox.checked))

                            if(e1bitComboBox.value !== channelComboBox.channel.e1bit)
                                device.send(card.commandToSetE1Bit(channelComboBox.currentIndex, e1bitComboBox.value))

                            if(sensitivityChanged) {
                                device.send(card.commandToSetSensitivity(channelComboBox.currentIndex,
                                                                         failureTransmEndSensitivitySlider.currentValue,
                                                                         crcErrorsSensitivitySlider.currentValue,
                                                                         lossOfMultiFrameSensitivitySlider.currentValue,
                                                                         lossOfFrameSensitivitySlider.currentValue,
                                                                         hdb3ErrorSensitivitySlider.currentValue
                                                                         ))
                            }

                            //                            channelSettingsDialog.close()
                        }
                    }
                }
            }

            Flickable {
                id: flickable
                width: parent.width; height: parent.height
                contentHeight: column.height
                boundsBehavior: Flickable.StopAtBounds
                clip: true

                interactive: contentHeight > height
                ScrollBar.vertical: ScrollBar { id: scrollbar; policy: ScrollBar.AlwaysOn; visible: flickable.interactive }

                contentWidth: scrollbar.visible ? width - scrollbar.width
                                                : width

                ColumnLayout {
                    id: column
                    anchors { left: parent.left; right: parent.right; margins: Theme.indent; }
                    spacing: Theme.middleIndent

                    enabled: page.controlRights

                    CheckBox {
                        id: measurementModeCheckBox
                        Layout.fillWidth: true
                        Layout.maximumWidth: implicitWidth
                        topInset: 0; bottomInset: 0
                        text: qsTr("Measurement mode")
                        Binding {
                            target: measurementModeCheckBox
                            property: "checked"
                            value: channelComboBox.channel.measurementMode
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        color: Material.accentColor
                        wrapMode: Label.WordWrap
                        text: qsTr("Bit for SSM transmission")
                    }

                    Controls.ComboBox {
                        id: e1bitComboBox
                        topInset: 0; bottomInset: 0
                        Layout.fillWidth: true
                        focus: true

                        Binding {
                            target: e1bitComboBox
                            property: "value"
                            value: channelComboBox.channel.e1bit
                        }

                        model: [
                            Vch003.E1Bit.Sa4,
                            Vch003.E1Bit.Sa5,
                            Vch003.E1Bit.Sa6,
                            Vch003.E1Bit.Sa7,
                            Vch003.E1Bit.Sa8
                        ]

                        delegateTextFunc: Vch003.e1bitToString
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Label.WordWrap
                        color: Material.accentColor
                        text: qsTr("Sensitivity to failure at the transmitting end")
                    }

                    QCC2.Slider {
                        id: failureTransmEndSensitivitySlider
                        Layout.fillWidth: true
                        model: [ qsTr("Off"), 0, 1, 2, 3 ]
                        currentIndex: channelComboBox.channel.failureTransmEndSensitivity === Vch003.Sensitivity.Off ? 0
                                                                                                                     : channelComboBox.channel.failureTransmEndSensitivity + 1
                        property int currentValue: currentIndex == 0 ? Vch003.Sensitivity.Off
                                                                     : model[currentIndex]
                    }

                    Label {
                        Layout.fillWidth: true
                        color: Material.accentColor
                        wrapMode: Label.WordWrap
                        text: qsTr("Sensitivity to CRC errors")
                    }

                    QCC2.Slider {
                        id: crcErrorsSensitivitySlider
                        Layout.fillWidth: true
                        model: [ qsTr("Off"), 0, 1, 2, 3 ]
                        currentIndex: channelComboBox.channel.crcErrorsSensitivity === Vch003.Sensitivity.Off ? 0
                                                                                                              : channelComboBox.channel.crcErrorsSensitivity + 1
                        property int currentValue: currentIndex == 0 ? Vch003.Sensitivity.Off
                                                                     : model[currentIndex]
                    }

                    Label {
                        Layout.fillWidth: true
                        color: Material.accentColor
                        wrapMode: Label.WordWrap
                        text: qsTr("Sensitivity to loss of multiframe synchronization")
                    }

                    QCC2.Slider {
                        id: lossOfMultiFrameSensitivitySlider
                        Layout.fillWidth: true
                        model: [ qsTr("Off"), 0, 1, 2, 3 ]
                        currentIndex: channelComboBox.channel.lossOfMultiFrameSensitivity === Vch003.Sensitivity.Off ? 0
                                                                                                                     : channelComboBox.channel.lossOfMultiFrameSensitivity + 1
                        property int currentValue: currentIndex == 0 ? Vch003.Sensitivity.Off
                                                                     : model[currentIndex]
                    }

                    Label {
                        Layout.fillWidth: true
                        color: Material.accentColor
                        wrapMode: Label.WordWrap
                        text: qsTr("Sensitivity to loss of frame synchronization")
                    }

                    QCC2.Slider {
                        id: lossOfFrameSensitivitySlider
                        Layout.fillWidth: true
                        model: [ qsTr("Off"), 0, 1, 2, 3 ]
                        currentIndex: channelComboBox.channel.lossOfFrameSensitivity === Vch003.Sensitivity.Off ? 0
                                                                                                                : channelComboBox.channel.lossOfFrameSensitivity + 1
                        property int currentValue: currentIndex == 0 ? Vch003.Sensitivity.Off
                                                                     : model[currentIndex]
                    }

                    Label {
                        Layout.fillWidth: true
                        color: Material.accentColor
                        wrapMode: Label.WordWrap
                        text: qsTr("HDB3 coding error sensitivity") // чувствительности к ошибкам HDB3 кодирования
                    }

                    QCC2.Slider {
                        id: hdb3ErrorSensitivitySlider
                        Layout.fillWidth: true
                        model: [ 0, 1, 2, 3 ]
                        currentIndex: channelComboBox.channel.hdb3ErrorSensitivity === Vch003.Sensitivity.Off ? -1
                                                                                                              : channelComboBox.channel.hdb3ErrorSensitivity
                        property int currentValue: currentIndex == -1 ? channelComboBox.channel.hdb3ErrorSensitivity
                                                                      : model[currentIndex]
                    }
                }   // Column
            }   // Flickable


        }
    }
}

