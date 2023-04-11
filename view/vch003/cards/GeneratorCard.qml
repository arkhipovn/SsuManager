import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Theme 1.0
import Models 1.0

import SSU 1.0

import QtQuick.Layouts 1.12

MainCard {
    numberOfSeats: 2

    Controls.Led {
        id: powerLed
        y: 40 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Power")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[0]

        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
    }

    ColumnLayout {
        y: 160 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            title: qsTr("Warmup")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            title: qsTr("Tracking")  //qsTr("Tracking/Holdover")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[2]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    Column {
        y: 340 * imageSlotHeightCoef
        x: powerLed.x
        spacing: viewSettings.itemSpacing(imageSlotWidthCoef, imageSlotHeightCoef)

        Column {
            spacing: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
                text: qsTr("Channel: 1-4/G/H/F")
            }

            Controls.Segment7Led {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(imageSlotHeightCoef * 75, imageSlotWidthCoef * 75)
                height: width * 1.37

                value: {
                    if(cardModel === null || cardModel.pllState === undefined)
                        return "-"
                    switch(cardModel.pllState) {
                    case SSU.PllState.Warmup:
                        return "-"
                    case SSU.PllState.Freerun:
                        return "F"
                    case SSU.PllState.Holdover:
                        return "X"//"H"     // Это из-за того, что этот шрифт H рисует, как h, а X -> H
                    default:
                        return device == null ? ""
                                              : device.channelDescription.cardInputChannelToString(cardModel.type, cardModel.activeChannel, ChannelDescription.NumberOnly)
                    }
                }

                highlightDotAtEnd: false
                numberOfDigits: 1
                color: "#5E6954"
                valueColor: "#07E600"
                offIndicatorColor:  "#747F6C"
            }
        }

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
                text: qsTr("Slot: A00-A05")
            }

            Controls.Segment7Led {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(imageSlotHeightCoef * 75, imageSlotWidthCoef * 75)
                height: width * 1.37

                value: {
                    if(cardModel === null || cardModel.pllState === undefined)
                        return "-"
                    switch(cardModel.pllState) {
                    case SSU.PllState.Warmup:
                    case SSU.PllState.Freerun:
                    case SSU.PllState.Holdover:
                        return "-"
                    default:
                        return device == null ? ""
                                              : device.channelDescription.cardInputChannelSlot(cardModel.type, cardModel.activeChannel, ChannelDescription.NumberOnly)
                    }
                }

                highlightDotAtEnd: false
                numberOfDigits: 1
                color: "#5E6954"
                valueColor: "#07E600"
                offIndicatorColor:  "#747F6C"
            }
        }
    }

    Controls.Led {
        y: 745 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Alarm")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[3]

        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
    }




    //    Label {
    //        background: Rectangle { border.width: 1}
    //        anchors.horizontalCenter: parent.horizontalCenter
    //        width: parent.width
    //        anchors.bottom: parent.bottom
    //        wrapMode: Label.WordWrap
    //        font.bold: true
    //        text: qsTr("Устройство сетевой синхронизации VCH-003")    //@N addTr
    //    }
}




