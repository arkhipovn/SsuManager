import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import SSU 1.0
import Theme 1.0

import "../private/"
import Models 1.0

Item {
    id: page

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    Controls.GroupBox {
        id: paramGroupBox
        width: Math.min(parent.width, Theme.pixelSize * 50)
        leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
        spacing: Theme.smallIndent

        label: Controls.TableHeader {
            width: paramGroupBox.width
            leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
            spacing: Theme.smallIndent
            implicitHeight: implicitContentHeight

            Item { Layout.preferredWidth: Theme.textFieldWidth; }

            Controls.TableCell {
                Layout.preferredWidth: 1
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Configuration")
            }

            Controls.TableCell {
                Layout.preferredWidth: 1
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Priority")
            }

            Controls.TableCell {
                Layout.preferredWidth: 1
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Wait to restore time")
            }
        }   // Controls.RowLayout signal

        contentItem: Column {
            id: inputChannelListColumn
            spacing: Theme.smallIndent

            Repeater {
                clip: true
                model: card.inputChannels
                delegate: inputChannelDelegate
            }
        }
    }

    RowLayout {
        anchors.top: paramGroupBox.bottom
        width: paramGroupBox.width

        QCC.SymbolButton {
            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
            Layout.alignment: Qt.AlignVCenter
            text: card.measurementMode ? qsTr("Measurement mode enabled")
                                       : qsTr("Measurement mode off")

            clickable: page.controlRights
            toolTip.text: page.controlRights ? (card.measurementMode ? qsTr("Click to turn off measurement mode")
                                                                : qsTr("Click to turn on measurement mode"))
                                        : ""

            onClicked: measurementModePopup.open()

            Dialogs.ConfirmationPopup {
                id: measurementModePopup
                text: card.measurementMode ? qsTr("Are you sure you want to turn off measurement mode?")
                                           : qsTr("Are you sure you want to turn on measurement mode?")
                onClicked: device.send(card.commandToSetMeasurementMode(!card.measurementMode))
            }
        }

        QCC.SymbolButton {
            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Delay to remove the ban by measurements") + " " + card.delayToRemoveBanByMeasurements + " " + qsTr("s")
            clickable: page.controlRights
            toolTip.text: page.controlRights ? qsTr("Click to change the ban removal delay by measurements")
                                        : ""

            onClicked: controlPopup.openPopup(measRestoreTimeSettingsComponent, -1, qsTr("Delay to remove the ban by measurements"))

        }

        Item { Layout.fillWidth: true; }
    }

    Component {
        id: inputChannelDelegate

        ChannelPane {
            id: channelPane
            width: inputChannelListColumn.width

            readonly property int numberOfChannel: index
            readonly property var channel: modelData

            ChannelStatusLabel {
                id: channelStateButton
                Layout.preferredWidth: Theme.textFieldWidth
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                //@N gnss channel name
                text: device == null ? ""
                                     : device.channelDescription.cardInputChannelToString(card.type, channelPane.numberOfChannel, ChannelDescription.Short)

                backgroundColor: {
                    var led = card.leds.data[index + 1]
                    if(!led.power)
                        return Material.color(Material.Grey)
                    if(Qt.colorEqual(led.color, "green"))
                        return Material.color(Material.LightGreen)
                    else if(Qt.colorEqual(led.color, "yellow"))
                        return Material.color(Material.Orange)
                    return led.color
                }

                toolTip.text: text + " " + (channel.state ? (channel.configuration ? qsTr("OK")
                                                                                   : qsTr("Off"))
                                                          : qsTr("absent"))

                ActiveIconLoader {
                    id: activeIconLoader
                    active: channelPane.numberOfChannel === card.activeChannel
                }   // ActiveIconLoader

                Controls.ToolTip {
                    x: channelStateButton.toolTip.x; y: channelStateButton.toolTip.y + channelStateButton.toolTip.height + Theme.smallIndent
                    visible: channelStateButton.hovered && channelStateButton.clickable
                    delay: 600
                    text: qsTr("Click to set active channel")
                }

                clickable: channel.state && channel.configuration && !activeIconLoader.active
                onClicked: changeActiveChannelPopup.open()

                Dialogs.ConfirmationPopup {
                    id: changeActiveChannelPopup
                    text: qsTr("Are you sure you want to change the active channel to") + " <b>" + channelStateButton.text + "</b>?"
                    onClicked: device.send(card.commandToSetActiveSignal(index))
                }
            }

            ItemDelegate {
                id: channelConfigurationItem
                Layout.fillWidth: true; Layout.fillHeight: true
                padding: 0
                topPadding: 0; bottomPadding: 0
                topInset: 0; bottomInset: 0

                contentItem: Item {
                    ColumnLayout {
                        width: parent.width;
                        spacing: 0
                        anchors.verticalCenter: parent.verticalCenter

                        Controls.TableCell {
                            Layout.fillWidth: true
                            text: channel.configuration ? qsTr("On", "ChannelConfig") : qsTr("Off", "ChannelConfig")
                            font.pixelSize: Theme.pixelSize * 0.8
                        }

                        QCC.ArrowItem {
                            Layout.fillWidth: true; implicitHeight: Theme.rowHeight * 0.4
                            lineColor: Material.color(Material.BlueGrey)
                            indicatorForm: channel.configuration ? QCC.ArrowItem.Arrow
                                                                 : QCC.ArrowItem.Cross
                        }
                    }
                }

                Controls.ToolTip {
                    id: channelTypeToolTip
                    visible: channelConfigurationItem.hovered && page.controlRights
                    text: qsTr("Click to set configuration")
                }

                onClicked: {
                    if(page.controlRights) controlPopup.openPopup(configurationSettingsComponent, channelPane.numberOfChannel,
                                                                  channelStateButton.text + ". " + qsTr("Configuration"))
                }
            }

            Controls.ValueLabel {
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.preferredWidth: 1
                horizontalAlignment: Label.AlignLeft
                text: channel.priority + 1  // +1 чтобы отображалось начиная с 1
                clickable: page.controlRights && card.mode === SSU.WorkingMode.AutoMode

                toolTip.text: qsTr("Click to set priority")
                onClicked: controlPopup.openPopup(prioritySettingsComponent, channelPane.numberOfChannel,
                                                  channelStateButton.text + ". " + qsTr("Priority"))
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


    Component {
        id: prioritySettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.inputChannels[numberOfChannel].priority
                model: device == null ? null
                                      : device.options.kNumberOfGnssCardInChannels
                delegateTextFunc: function(value) { return value + 1; }
            }

            Button {
                anchors.right: parent.right
                enabled: comboBox.value !== card.inputChannels[numberOfChannel].priority
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetChannelPriority(numberOfChannel, comboBox.value))
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
                    // visible: card.inputChannels[numberOfChannel].state & Vch003.RecoveryDelay
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

    Component {
        id: configurationSettingsComponent

        RowLayout {
            spacing: Theme.middleIndent

            Button {
                Layout.fillWidth: true
                Layout.maximumWidth: implicitWidth
                Layout.alignment: Qt.AlignHCenter
                text: card.inputChannels[numberOfChannel].configuration ? qsTr("Turn off") : qsTr("Turn on")

                onClicked: {
                    device.send(card.commandToChannelConfiguration(numberOfChannel, !card.inputChannels[numberOfChannel].configuration))
                    controlPopup.close()
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.maximumWidth: implicitWidth
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Turn on all")

                onClicked: {
                    for(let i = 0; i < card.inputChannels.length; ++i) {
                        device.send(card.commandToChannelConfiguration(i, true))
                    }
                    controlPopup.close()
                }
            }

            Button {
                Layout.fillWidth: true
                Layout.maximumWidth: implicitWidth
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Turn off all")

                onClicked: {
                    for(let i = 0; i < card.inputChannels.length; ++i) {
                        device.send(card.commandToChannelConfiguration(i, false))
                    }
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: measRestoreTimeSettingsComponent

        Column {
            TextField {
                id: textField
                width: parent.width
                focus: true
                selectByMouse: true
                wrapMode: TextArea.Wrap
                text: card.delayToRemoveBanByMeasurements

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

            Button {
                anchors.right: parent.right
                enabled: textField.text != card.delayToRemoveBanByMeasurements && textField.acceptableInput
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetDelayToRemoveBanByMeasurements(textField.text))
                    controlPopup.close()
                }
            }
        }
    }


}
