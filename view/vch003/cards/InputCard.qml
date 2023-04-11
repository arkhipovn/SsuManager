import QtQuick 2.12
import Controls 1.0 as Controls

import QtQuick.Layouts 1.12

MainCard {
    id: card

    Controls.Led {
        y: 40 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Power")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[0]

        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
    }

    ColumnLayout {
        y: 160 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * 2 * imageSlotHeightCoef
        spacing: 0

        Repeater {
            id: repeater
            model: 4
            Controls.Led {
                Layout.alignment: Qt.AlignHCenter | (index == 0 ? Qt.AlignTop
                                                                : index == repeater.count - 1 ? Qt.AlignBottom
                                                                                              : Qt.AlignVCenter)

                title: qsTr("Inp") + " " + (index + 1)
                obj: cardModel === null || cardModel.leds === undefined ? null
                                                                        : cardModel.leds.data[index + 1]

                font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
                ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            }
        }
    }

    Controls.Led {
        y: 745 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Alarm")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[5]

        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
    }
}


