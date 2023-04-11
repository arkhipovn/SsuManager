import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import Models 1.0

import SSU 1.0
import Theme 1.0

import "private/"

CardPage {
    id: page

    footerData: [
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: card.active ? qsTr("Active", "CardMode") : qsTr("Passive")

            clickable: {
                if(device == null || !card.active || !card.synchronization)
                return false

                let list = device.panel.inventory.hasData ? device.panel.getCardGroup(groupNumber, card.type)
                                                          : []
                let count = 0;
                for(let i = 0; i < list.length; ++i) {
                    if(list[i].synchronization)
                    count++
                }
                return count > 1
            }

            toolTip.text: !card.active ? ""
                                       : clickable ? qsTr("Click to give activity")
                                                   : !card.active ? qsTr("Passive")
                                                                  : !card.synchronization ? qsTr("Synchronization error. Unable to release activity")
                                                                                          : qsTr("Passive card not available")
            textColor: Material.accentColor

            onClicked: giveActivityConfirmPopup.open()

            Dialogs.ConfirmationPopup {
                id: giveActivityConfirmPopup
                text: qsTr("Are you sure you want to change the active card?")
                onClicked: device.send(card.commandToGiveActivity())
            }
        }
    ]

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..


    header: Pane {
        spacing: 0
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent;

        contentItem: RowLayout {
            id: headerContent
            spacing: 0

            Loader {
                active: !card.synchronization
                visible: active

                sourceComponent: Label {
                    text: qsTr("Sync Failure")
                    color: Material.color(Material.Red)
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                text: qsTr("Group") + " " + (groupNumber + 1)
                font.bold: true
                color: Material.accentColor
            }
        }
    }   // header Pane

    Controls.GroupBox {
        id: outputParamGroupBox
        width: parent.width; height: parent.height

        Flickable {
            width: parent.width; height: parent.height
            boundsBehavior: ListView.StopAtBounds
            flickableDirection: ListView.VerticalFlick
            contentHeight: grid.height

            clip: true

            GridLayout {
                id: grid
                clip: true
                width: parent.width
                flow: GridLayout.TopToBottom
                rows: 10
                rowSpacing: Theme.smallIndent; columnSpacing: Theme.smallIndent

                Repeater {
                    model: card.outputChannels
                    delegate: outputChannelDelegate
                }
            }
        }
    }   // Output GroupBox

    Component {
        id: outputChannelDelegate

        ChannelPane {
            id: channelPane

            Layout.fillWidth: true;
            spacing: 0

            readonly property int numberOfChannel: index
            readonly property var channel: modelData

            ChannelStatusLabel {
                id: channelStateButton
                Layout.preferredWidth: Theme.textFieldWidth
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: device == null ? ""
                                     : device.channelDescription.cardOutputChannelToString(card.type, numberOfChannel, ChannelDescription.Short)
                backgroundColor: channel.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                       Material.color(Material.LightGreen)
                toolTip.text: channelStateButton.text +
                              "<p>" + channelTypeToolTip.text //SSU.signalTypeToString(channel.type)
            }

            Label {
                text: channel.holdoverModeEnabled ? "\uf14a" : "\uf096"
                font.pixelSize: Theme.pixelSize * 1.5
                leftPadding: Theme.smallIndent * 0.5; rightPadding: Theme.smallIndent * 0.5
                color: Material.backgroundDimColor

                background: MouseArea {
                    id: mouseArea
                    hoverEnabled: true
                    onClicked: if(page.controlRights) permOnHoldConfirmPopup.open()
                }

                Controls.ToolTip {
                    id: holdoverModeEnabledToolTip
                    visible: mouseArea.containsMouse
                    text: qsTr("Permission on holdover")
                }

                Controls.ToolTip {
                    x: holdoverModeEnabledToolTip.x; y: holdoverModeEnabledToolTip.y + holdoverModeEnabledToolTip.height + Theme.smallIndent
                    visible: mouseArea.containsMouse && page.controlRights
                    delay: 600
                    text: qsTr("Click to change permission on holdover")
                }

                Dialogs.ConfirmationPopup {
                    id: permOnHoldConfirmPopup
                    text: qsTr("Are you sure you want to change permission on holdover?")
                    onClicked: device.send(card.commandToSetPermissionOnHoldover(index, !channel.holdoverModeEnabled))
                }
            }

            ItemDelegate {
                id: channelTypeItem
                Layout.fillWidth: true; Layout.fillHeight: true
                padding: 0
                topPadding: 0; bottomPadding: 0
                topInset: 0; bottomInset: 0

                property bool clickable: page.controlRights

                contentItem: Item {
                    ColumnLayout {
                        width: parent.width;
                        spacing: 0
                        anchors.verticalCenter: parent.verticalCenter

                        Controls.TableCell {
                            Layout.fillWidth: true
                            text: SSU.signalTypeToString(channel.type)
                            font.pixelSize: Theme.pixelSize * 0.8
                        }

                        QCC.ArrowItem {
                            Layout.fillWidth: true; implicitHeight: Theme.rowHeight * 0.4
                            lineColor: Material.color(Material.BlueGrey)
                            indicatorForm: channel.state ? QCC.ArrowItem.Arrow
                                                         : QCC.ArrowItem.Cross
                        }
                    }
                }

                Controls.ToolTip {
                    id: channelTypeToolTip
                    visible: channelTypeItem.hovered
                    text: SSU.signalTypeToString(channel.type) +
                          (channel.state ? "" : "<p>" + qsTr("No output signal"))
                }

                Controls.ToolTip {
                    x: channelTypeToolTip.x; y: channelTypeToolTip.y + channelTypeToolTip.height + Theme.smallIndent
                    visible: channelTypeItem.hovered && channelTypeItem.clickable
                    delay: 600
                    text: qsTr("Click to set signal type")
                }

                onClicked: {
                    if(channelTypeItem.clickable) controlPopup.openPopup(channelTypeSettingsComponent, channelPane.numberOfChannel,
                                                                         channelStateButton.text + ". " + qsTr("Signal type"))
                }
            }
        }
    }   // outputChannelDelegate


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
    }   // SettingsPopup

    Component {
        id: channelTypeSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.outputChannels[numberOfChannel].type
                model: [
                    SSU.SignalType.Off,
                    SSU.SignalType.KHz2048,
                    SSU.SignalType.E1
                ]
                delegateTextFunc: SSU.signalTypeToString

                readonly property int numberOfChannelsInGroup: Math.round(card.outputChannels.length / 2)
            }

            RowLayout {
                width: parent.width


                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: applyToAllChannelsButton
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth
                    text: qsTr("Apply to all")

                    Controls.ToolTip {
                        visible: applyToAllChannelsButton.hovered
                        text: applyToAllChannelsButton.text
                    }

                    onClicked: {
                        for(let i = 0; i < card.outputChannels.length; ++i) {
                            device.send(card.commandToSetTypeOutputChannel(i, comboBox.value))
                        }
                    }
                }

                Button {
                    id: applyToGroupChannelsButton
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth

                    text: qsTr("To") + " " +
                          (numberOfChannel >= comboBox.numberOfChannelsInGroup ? (comboBox.numberOfChannelsInGroup + 1) + "-" + (card.outputChannels.length)
                                                                               : "1-" + comboBox.numberOfChannelsInGroup) +
                          " " + qsTr("channels")

                    Controls.ToolTip {
                        visible: applyToGroupChannelsButton.hovered
                        text: applyToGroupChannelsButton.text
                    }

                    onClicked: {
                        let start = numberOfChannel >= comboBox.numberOfChannelsInGroup ? comboBox.numberOfChannelsInGroup
                                                                                        : 0
                        let end = numberOfChannel >= comboBox.numberOfChannelsInGroup ? card.outputChannels.length
                                                                                      : comboBox.numberOfChannelsInGroup
                        for(let i = start; i < end; ++i)
                            device.send(card.commandToSetTypeOutputChannel(i, comboBox.value))
                    }
                }
            }

            Button {
                anchors.right: parent.right

                enabled: comboBox.value !== card.outputChannels[numberOfChannel].type
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetTypeOutputChannel(numberOfChannel, comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }

}
