import QtQuick 2.12
import QtQuick.Controls 2.12

import Device 1.0 as D
import Theme 1.0

import Controls 1.0 as Controls

D.Card {
    numberOfSeats: 2

    clickable: false
    source: "qrc:/images/vch003/PowerCard.jpg"

    Item {
        y: 130 * imageSlotHeightCoef
        //@N Что-то нужно сделать с panel... Тк сейчас зависит от id все..
        width: panel.slotWidth// parent.width / cardModel.numberOfSeats - (cardModel.numberOfSeats - 1)

        Controls.Led {
            y: 40 * imageSlotHeightCoef
            anchors.horizontalCenter: parent.horizontalCenter

            title: qsTr("Power")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[0]

            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            y: 160 * imageSlotHeightCoef
            anchors.horizontalCenter: parent.horizontalCenter

            title: qsTr("Fuse")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]

            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    Column {
        anchors { bottom: titleLabel.top; bottomMargin: 80 * imageSlotHeightCoef; horizontalCenter: parent.horizontalCenter; }
        spacing: 15 * imageSlotHeightCoef

        Label {
            id: descLabel
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            text: qsTr("F 10 A 250 V")
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 85 * imageSlotHeightCoef; height: width; radius: width * 0.5
            color: cardModel.supply ? (cardModel.fuse ? "#232323"
                                                      : "red")
                                    : "#D1D1D1"

            Rectangle {
                visible: cardModel.fuse && cardModel.supply
                anchors { fill: parent; margins: 8 * imageSlotHeightCoef; }
                radius: width * 0.5
                color: "#545454"
            }

            MouseArea {
                id: mouse
                anchors.fill: parent
                hoverEnabled: true
            }

            //            Controls.ToolTip {
            //                visible: mouse.containsMouse
            //                text: "123"
            //            }
        }
    }

    // title
    Label {
        id: titleLabel
        x: 3 * imageSlotWidthCoef; y: 633 * imageSlotHeightCoef; z: 2
        width: 232 * imageSlotWidthCoef; height: 77 * imageSlotHeightCoef
        text: viewSettings.cardTitle(cardModel.type)

        font { pixelSize: viewSettings.bigPixelSize(imageSlotWidthCoef, imageSlotHeightCoef); weight: Font.DemiBold }

        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
    }
}

