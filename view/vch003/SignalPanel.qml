import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import SSU 1.0
import Theme 1.0

import Controls 1.0 as Controls

import "pages/private/InputChannelState.js" as InputChannelState

Pane {
    id: signalPanelPane
    property var device: null

    font.family: viewSettings.panelFontFamily
    font.pixelSize: Math.min(signalPanelImage.imageWidthCoef * 16, signalPanelImage.imageHeightCoef * 42)
    Material.foreground: "#D2E5DC"

    function findCard(group) {
        if(signalPanelPane.device == null)
            return null


        if(signalPanelPane.device.panel.inventory.hasData) {
            for(let i = 0; i < group.length; ++i) {
                let card = signalPanelPane.device.panel.inventory.card(group[i])
                if(card !== null)
                    return card
            }
        }
        return null
    }

    background: Image {
        id: signalPanelImage
        source: "qrc:/images/vch003/SignalPanel_ru.png"
        smooth: true
        mipmap: true
        antialiasing: true

        readonly property real imageWidthCoef: width / sourceSize.width
        readonly property real imageHeightCoef: height / sourceSize.height

        // Output A12 - A13
        Label {
            id: labelOuputGroup1
            x: 502 * signalPanelImage.imageWidthCoef
            y: 43 * signalPanelImage.imageHeightCoef - height / 2
            width: 294 * signalPanelImage.imageWidthCoef
            text: qsTr("Outputs 2048 kHz/kbit/s") + " A12 - A13"
            horizontalAlignment: Qt.AlignHCenter
        }

        RowLayout {
            id: outputGroup1Layout
            readonly property var card: findCard([12, 13])

            x: 40 * signalPanelImage.imageWidthCoef
            y: 65 * signalPanelImage.imageHeightCoef
            width: 1220 * signalPanelImage.imageWidthCoef; height: 210 * signalPanelImage.imageHeightCoef

            Repeater {
                model: 10

                Controls.Rs232 {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    title: (index * 2 + 1) + "-" + (index * 2 + 2)

                    readonly property var channel1: outputGroup1Layout.card === null ? null
                                                                                     : outputGroup1Layout.card.outputChannels[index * 2]
                    readonly property var channel2: outputGroup1Layout.card === null ? null
                                                                                     : outputGroup1Layout.card.outputChannels[index * 2 + 1]

                    color1: channel1 === null ? Material.color(Material.Grey)
                                              : channel1.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    color2: channel2 === null ? Material.color(Material.Grey)
                                              : channel2.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    toolTip.text: {
                        if(signalPanelPane.device == null)
                            return null

                        let str = labelOuputGroup1.text + "\n"
                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2)
                        if(channel1 !== null) str += ": " + SSU.signalTypeToString(channel1.type)
                        str += "\n"

                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2 + 1)
                        if(channel2 !== null) str += ": " + SSU.signalTypeToString(channel2.type)
                        return str
                    }
                }
            }
        }

        // Output A14 - A15
        Label {
            id: labelOuputGroup2
            x: 1742 * signalPanelImage.imageWidthCoef
            y: 43 * signalPanelImage.imageHeightCoef - height / 2
            width: 294 * signalPanelImage.imageWidthCoef
            text: qsTr("Outputs 2048 kHz/kbit/s") + " A14 - A15"
            horizontalAlignment: Qt.AlignHCenter
        }

        RowLayout {
            id: outputGroup2Layout
            readonly property var card: findCard([14, 15])

            x: 1280 * signalPanelImage.imageWidthCoef
            y: 65 * signalPanelImage.imageHeightCoef
            width: 1220 * signalPanelImage.imageWidthCoef; height: 210 * signalPanelImage.imageHeightCoef

            Repeater {
                model: 10

                Controls.Rs232 {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    title: (index * 2 + 1) + "-" + (index * 2 + 2)

                    readonly property var channel1: outputGroup2Layout.card === null ? null
                                                                                     : outputGroup2Layout.card.outputChannels[index * 2]
                    readonly property var channel2: outputGroup2Layout.card === null ? null
                                                                                     : outputGroup2Layout.card.outputChannels[index * 2 + 1]

                    color1: channel1 === null ? Material.color(Material.Grey)
                                              : channel1.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    color2: channel2 === null ? Material.color(Material.Grey)
                                              : channel2.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    toolTip.text: {
                        if(signalPanelPane.device == null)
                            return null

                        let str = labelOuputGroup2.text + "\n"
                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2)
                        if(channel1 !== null) str += ": " + SSU.signalTypeToString(channel1.type)
                        str += "\n"

                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2 + 1)
                        if(channel2 !== null) str += ": " + SSU.signalTypeToString(channel2.type)
                        return str
                    }
                }
            }
        }


        // Output A10 - A11

        Label {
            id: labelOuputGroup3
            x: 502 * signalPanelImage.imageWidthCoef
            y: 283 * signalPanelImage.imageHeightCoef - height / 2
            width: 294 * signalPanelImage.imageWidthCoef
            text: qsTr("Outputs 2048 kHz/kbit/s") + " A10 - A11"
            horizontalAlignment: Qt.AlignHCenter
        }

        RowLayout {
            id: outputGroup3Layout
            readonly property var card: findCard([10, 11])

            x: 40 * signalPanelImage.imageWidthCoef
            y: 303 * signalPanelImage.imageHeightCoef
            width: 1220 * signalPanelImage.imageWidthCoef; height: 210 * signalPanelImage.imageHeightCoef

            Repeater {
                model: 10

                Controls.Rs232 {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    title: (index * 2 + 1) + "-" + (index * 2 + 2)

                    readonly property var channel1: outputGroup3Layout.card === null ? null
                                                                                     : outputGroup3Layout.card.outputChannels[index * 2]
                    readonly property var channel2: outputGroup3Layout.card === null ? null
                                                                                     : outputGroup3Layout.card.outputChannels[index * 2 + 1]

                    color1: channel1 === null ? Material.color(Material.Grey)
                                              : channel1.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    color2: channel2 === null ? Material.color(Material.Grey)
                                              : channel2.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    toolTip.text: {
                        if(signalPanelPane.device == null)
                            return null

                        let str = labelOuputGroup3.text + "\n"
                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2)
                        if(channel1 !== null) str += ": " + SSU.signalTypeToString(channel1.type)
                        str += "\n"

                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2 + 1)
                        if(channel2 !== null) str += ": " + SSU.signalTypeToString(channel2.type)
                        return str
                    }
                }
            }
        }

        // Output A16 - A17

        Label {
            id: labelOuputGroup4
            x: 1742 * signalPanelImage.imageWidthCoef
            y: 283 * signalPanelImage.imageHeightCoef - height / 2
            width: 294 * signalPanelImage.imageWidthCoef
            text: qsTr("Outputs 2048 kHz/kbit/s") + " A16 - A17"
            horizontalAlignment: Qt.AlignHCenter
        }

        RowLayout {
            id: outputGroup4Layout
            readonly property var card: findCard([16, 17])

            x: 1280 * signalPanelImage.imageWidthCoef
            y: 303 * signalPanelImage.imageHeightCoef
            width: 1220 * signalPanelImage.imageWidthCoef; height: 210 * signalPanelImage.imageHeightCoef

            Repeater {
                model: 10

                Controls.Rs232 {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    title: (index * 2 + 1) + "-" + (index * 2 + 2)

                    readonly property var channel1: outputGroup4Layout.card === null ? null
                                                                                     : outputGroup4Layout.card.outputChannels[index * 2]
                    readonly property var channel2: outputGroup4Layout.card === null ? null
                                                                                     : outputGroup4Layout.card.outputChannels[index * 2 + 1]

                    color1: channel1 === null ? Material.color(Material.Grey)
                                              : channel1.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    color2: channel2 === null ? Material.color(Material.Grey)
                                              : channel2.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                                       Material.color(Material.LightGreen)
                    toolTip.text: {
                        if(signalPanelPane.device == null)
                            return null

                        let str = labelOuputGroup4.text + "\n"
                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2)
                        if(channel1 !== null) str += ": " + SSU.signalTypeToString(channel1.type)
                        str += "\n"

                        str += signalPanelPane.device.channelDescription.cardOutputChannelToString(SSU.Output, index * 2 + 1)
                        if(channel2 !== null) str += ": " + SSU.signalTypeToString(channel2.type)
                        return str
                    }
                }
            }
        }


        // Input A00-A01

        Label {
            x: 566 * signalPanelImage.imageWidthCoef
            y: 530 * signalPanelImage.imageHeightCoef - height / 2
            width: 168 * signalPanelImage.imageWidthCoef
            text: qsTr("Inputs") + " A00 - A01"
            horizontalAlignment: Qt.AlignHCenter
        }

        GridLayout {
            id: inputGroup1Layout
            x: 413 * signalPanelImage.imageWidthCoef
            y: 555 * signalPanelImage.imageHeightCoef
            width: 475 * signalPanelImage.imageWidthCoef; height: 185 * signalPanelImage.imageHeightCoef

            rows: 3
            columns: signalPanelPane.device == null ? 0
                                                    : signalPanelPane.device.options.kNumberOfInputCardInChannels

            property var card: {
                if(signalPanelPane.device == null)
                    return null
                if(signalPanelPane.device.panel.inventory.hasData) {
                    var card = signalPanelPane.device.panel.inventory.card(0)
                    if(card !== null)
                        return card
                    return signalPanelPane.device.panel.inventory.card(1)
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels

                Controls.InputChannel {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    readonly property var channel: inputGroup1Layout.card === null ? null
                                                                                   : inputGroup1Layout.card.inputChannels[index]

                    color: inputGroup1Layout.card === null ? Material.color(Material.Grey)
                                                           : InputChannelState.stateColor(inputGroup1Layout.card.leds.data[index + 1])
                    toolTip.text: InputChannelState.stateToolTip(channel)
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels
                Label {
                    Layout.alignment: Qt.AlignCenter
                    wrapMode: Label.WordWrap
                    text: index + 1
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels

                Controls.InputChannel {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter
                    //                property var channel: {
                    //                    if(signalPanelPane.device == null)
                    //                        return null
                    //                    if(signalPanelPane.device.panel.inventory.hasData) {
                    //                        var card = signalPanelPane.device.panel.inventory.card(0)
                    //                        if(card === null)
                    //                            card = signalPanelPane.device.panel.inventory.card(1)
                    //                        if(card !== null)
                    //                            return card.inputChannels[index]
                    //                    }
                    //                    return null
                    //                }
                }
            }
        }

        Label {
            x: 566 * signalPanelImage.imageWidthCoef
            y: 759 * signalPanelImage.imageHeightCoef - height / 2
            width: 168 * signalPanelImage.imageWidthCoef
            text: qsTr("Statuses") + " A00 - A01"
            horizontalAlignment: Qt.AlignHCenter
        }




        // Input A02-A03
        Label {
            x: 1062 * signalPanelImage.imageWidthCoef
            y: 530 * signalPanelImage.imageHeightCoef - height / 2
            width: 168 * signalPanelImage.imageWidthCoef
            text: qsTr("Inputs") + " A02 - A03"
            horizontalAlignment: Qt.AlignHCenter
        }

        GridLayout {
            id: inputGroup2Layout
            x: 908 * signalPanelImage.imageWidthCoef
            y: 555 * signalPanelImage.imageHeightCoef
            width: 475 * signalPanelImage.imageWidthCoef; height: 185 * signalPanelImage.imageHeightCoef

            rows: 3
            columns: signalPanelPane.device == null ? 0
                                                    : signalPanelPane.device.options.kNumberOfInputCardInChannels

            property var card: {
                if(signalPanelPane.device == null)
                    return null
                if(signalPanelPane.device.panel.inventory.hasData) {
                    var card = signalPanelPane.device.panel.inventory.card(2)
                    if(card !== null)
                        return card
                    return signalPanelPane.device.panel.inventory.card(3)
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels

                Controls.InputChannel {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter

                    readonly property var channel: inputGroup2Layout.card === null ? null
                                                                                   : inputGroup2Layout.card.inputChannels[index]

                    color: inputGroup2Layout.card === null ? Material.color(Material.Grey)
                                                           : InputChannelState.stateColor(inputGroup2Layout.card.leds.data[index + 1])
                    toolTip.text: InputChannelState.stateToolTip(channel)
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels
                Label {
                    Layout.alignment: Qt.AlignCenter
                    wrapMode: Label.WordWrap
                    text: index + 1
                }
            }

            Repeater {
                model: signalPanelPane.device == null ? 0
                                                      : signalPanelPane.device.options.kNumberOfInputCardInChannels

                Controls.InputChannel {
                    Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignCenter
                    //                property var channel: {
                    //                    if(signalPanelPane.device == null)
                    //                        return null
                    //                    if(signalPanelPane.device.panel.inventory.hasData) {
                    //                        var card = device.panel.inventory.card(2)
                    //                        if(card === null)
                    //                            card = signalPanelPane.device.panel.inventory.card(3)
                    //                        if(card !== null)
                    //                            return card.inputChannels[index]
                    //                    }
                    //                    return null
                    //                }
                }
            }
        }

        Label {
            x: 1062 * signalPanelImage.imageWidthCoef
            y: 759 * signalPanelImage.imageHeightCoef - height / 2
            width: 168 * signalPanelImage.imageWidthCoef
            text: qsTr("Statuses") + " A02 - A03"
            horizontalAlignment: Qt.AlignHCenter
        }

        // GNSS A04 - A05

        Label {
            x: 1446 * signalPanelImage.imageWidthCoef
            y: 530 * signalPanelImage.imageHeightCoef - height / 2
            width: 140 * signalPanelImage.imageWidthCoef
            text: qsTr("GNSS") + " A04 - A05"
            horizontalAlignment: Qt.AlignHCenter
        }

        RowLayout {
            x: 1397 * signalPanelImage.imageWidthCoef
            y: 555 * signalPanelImage.imageHeightCoef
            width: 242 * signalPanelImage.imageWidthCoef; height: 70 * signalPanelImage.imageHeightCoef

            Controls.InputChannel {
                hoverEnabled: false
                Layout.preferredWidth: 75 * signalPanelImage.imageWidthCoef
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
            }
        }

        Label {
            x: 1783 * signalPanelImage.imageWidthCoef
            y: 530 * signalPanelImage.imageHeightCoef - height / 2
            width: 200 * signalPanelImage.imageWidthCoef
            text: qsTr("Control and management")
            horizontalAlignment: Qt.AlignHCenter
        }

        // Аварии

        ColumnLayout {
            x: 1666 * signalPanelImage.imageWidthCoef
            y: 548 * signalPanelImage.imageHeightCoef
            width: 210 * signalPanelImage.imageWidthCoef; height: 217 * signalPanelImage.imageHeightCoef

            Controls.Rs232 {
                Layout.preferredWidth: 182 * signalPanelImage.imageWidthCoef
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                orientation: Qt.Horizontal
                title: qsTr("Alarms")
                hoverEnabled: false
            }

            Controls.Rs232 {
                Layout.preferredWidth: 182 * signalPanelImage.imageWidthCoef
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                orientation: Qt.Horizontal
                title: qsTr("RS-232")
                hoverEnabled: false
            }
        }

        // Питание А
        Label {
            x: 244 * signalPanelImage.imageWidthCoef
            y: 537 * signalPanelImage.imageHeightCoef - height / 2
            width: 123 * signalPanelImage.imageWidthCoef
            wrapMode: Label.WordWrap
            text: qsTr("Power A Upper unit")
            horizontalAlignment: Qt.AlignHCenter
        }

        // Питание B
        Label {
            x: 2173 * signalPanelImage.imageWidthCoef
            y: 537 * signalPanelImage.imageHeightCoef - height / 2
            width: 123 * signalPanelImage.imageWidthCoef
            wrapMode: Label.WordWrap
            text: qsTr("Power B Lower unit")
            horizontalAlignment: Qt.AlignHCenter
        }
    }
}


