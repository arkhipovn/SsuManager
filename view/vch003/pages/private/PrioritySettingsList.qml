import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Theme 1.0
import Controls 1.0 as Controls

import SSU 1.0
import Models 1.0

ListView {
    id: prioritySettingsList

    spacing: Theme.middleIndent
    boundsBehavior: ListView.StopAtBounds
    flickableDirection: ListView.VerticalFlick
    headerPositioning: ListView.OverlayHeader
    delegate: inputGroupPriorityComponent

    property bool readOnly: false

    property var device: null
    property var card: null

    property var priorities: []

    signal changePriority(var index, var pri, var priPrev)

    property bool changed: false

    Component {
        id: inputGroupPriorityComponent

        Loader {
            id: inputGroupLoader
            width: ListView.view.contentWidth;

            readonly property var group: modelData

            active: inputGroupLoader.group.length > 0 &&
                    inputGroupLoader.group[0].numberOfSsuChannels.length > 0 &&
                    inputGroupLoader.group[0].numberOfSsuChannels[0] !== -1

            sourceComponent: bypassInputChannelComponent

            Component {
                id: bypassInputChannelComponent

                RowLayout {
                    id: inputGroupRow

                    // Нужно, чтобы взять SSM
                    readonly property var inputCard: {
                        if(device == null)
                            return null
                        device.panel.inventory.hasData
                        for(let i = 0; i < group.length; ++i) {
                            var card = device.panel.inventory.card(group[i].slot)
                            if(card !== null) {
                                return card
                            }
                        }
                        return null
                    }

                    Repeater {
                        model: group

                        delegate: Controls.GroupBox {
                            Layout.fillWidth: true;
                            title: SSU.addressToString(modelData.slot)
                            leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

                            contentItem: Column {
                                spacing: Theme.smallIndent

                                Repeater {
                                    model: modelData.numberOfCardInputs


                                    delegate: ChannelStatusLabel {
                                        id: channelStatusLabel
                                        readonly property var channel: card.inputChannels[modelData]

                                        priority: channel.currentPriority + 1
                                        text: device == null ? ""
                                                             : device.channelDescription.cardInputChannelToString(card.type, modelData, ChannelDescription.Short)
                                        backgroundColor: channel.ban ? Material.color(Material.Red)
                                                                     : Material.color(Material.LightGreen)

                                        toolTip.text: {
                                            if(device == null)
                                                return ""
                                            var name = device.channelDescription.cardInputChannelToString(card.type, modelData)
                                            return name + " - " +
                                                    (channel.ban ? qsTr("Ban") : qsTr("OK")) + "\n" +
                                                    (modelData === card.activeChannel ? (qsTr("Active") + "\n")
                                                                                      : modelData === card.backupChannel ? (qsTr("Backup") + "\n")
                                                                                                                         : "") +
                                                    (qsTr("Current priority") + ": " + channelStatusLabel.priority)
                                        }

                                        ActiveIconLoader {
                                            id: activeIconLoader
                                            symbol: modelData === card.activeChannel ? "A"
                                                                                     : modelData === card.backupChannel ? "B"
                                                                                                                        : ""
                                            active: symbol.length != 0
                                        }   // ActiveIconLoader
                                    }   // delegate
                                }   // Repeater
                            }   // Column
                        }   // Group box
                    }   // Repeater group


                    // Info group box
                    Controls.GroupBox {
                        id: paramGroupBox
                        Layout.fillWidth: true

                        label: Controls.TableHeader {
                            topPadding: 0; bottomPadding: 0
                            leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                            spacing: Theme.smallIndent
                            implicitHeight: implicitContentHeight
                            width: paramGroupBox.width

                            Controls.TableCell {
                                Layout.preferredWidth: 1
                                Layout.fillWidth: true; Layout.fillHeight: true
                                text: qsTr("Custom priority")
                            }

                            Controls.TableCell {
                                Layout.preferredWidth: 1
                                Layout.fillWidth: true; Layout.fillHeight: true
                                text: qsTr("SSM")
                            }
                        }   // GroupBox header

                        contentItem: Column {
                            spacing: Theme.smallIndent

                            Repeater {
                                model: group[0].numberOfCardInputs
                                delegate: ChannelPane {
                                    id: channelPane
                                    width: parent.width

                                    property bool completed: false

                                    Component.onCompleted: {
                                        prioritySettingsList.priorities.push(card.inputChannels[numberOfCardInput].priority)
                                        completed = true
                                    }

                                    readonly property int numberOfSsuInput: inputGroupLoader.group[0].numberOfSsuChannels[index]
                                    readonly property int numberOfCardInput: modelData
                                    readonly property var channel: card.inputChannels[numberOfCardInput]

                                    Controls.ComboBox {
                                        id: priComboBox
                                        topInset: 0; bottomInset: 0
                                        Layout.fillWidth: true; Layout.fillHeight: true
                                        Layout.preferredWidth: 1
                                        focus: true
                                        model: device == null ? null
                                                              : device.channelDescription.numberOfSsuInputChannels()
                                        delegateTextFunc: function(value) { return value + 1; }

                                        enabled: !readOnly
                                        Binding {
                                            target: priComboBox
                                            when: !prioritySettingsList.changed && completed
                                            property: "value"
                                            value: card.inputChannels[numberOfCardInput].priority
                                        }

                                        Connections {
                                            target: prioritySettingsList
                                            onChangePriority: {
                                                if(index == numberOfSsuInput)
                                                    return
                                                if(pri === priComboBox.value)
                                                    priComboBox.value = priPrev
                                            }
                                        }

                                        property int lastValue: value

                                        onActivated: {
                                            changed = true
                                            prioritySettingsList.changePriority(numberOfSsuInput, value, lastValue)
                                        }

                                        onValueChanged: {
                                            if(!completed)
                                                return
                                            lastValue = prioritySettingsList.priorities[numberOfSsuInput]
                                            prioritySettingsList.priorities[numberOfSsuInput] = value
                                        }
                                    }

                                    // SSM
                                    Controls.ValueLabel {
                                        Layout.fillWidth: true; Layout.fillHeight: true
                                        Layout.preferredWidth: 1
                                        horizontalAlignment: Label.AlignLeft

                                        function getInputSsm(index) {
                                            if(inputGroupRow.inputCard === null)
                                                return SSU.SSM.None

                                            switch(inputGroupRow.inputCard.type) {
                                            case SSU.Input:
                                                if(inputGroupRow.inputCard.inputChannels !== null &&
                                                        inputGroupRow.inputCard.inputChannels.length > index)
                                                {
                                                    return inputGroupRow.inputCard.inputChannels[index].ssm
                                                }
                                                break
                                            case SSU.Gnss:
                                                return inputGroupRow.inputCard.ssm
                                            default: break
                                            }
                                            return SSU.SSM.None
                                        }

                                        text: SSU.ssmToString(getInputSsm(index))
                                    }
                                }   // ChannelPane

                            }    // Info group Repeater
                        }   // Info group Column


                    }    // Info groupbox
                }   // Row Layout
            }


        }   // Loader

    }   // inputGroupDelegate Component





}

