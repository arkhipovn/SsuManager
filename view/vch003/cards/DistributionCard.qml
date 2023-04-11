import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls

MainCard {
    Controls.Led {
        id: powerLed
        anchors.horizontalCenter: parent.horizontalCenter
        y: 40 * imageSlotHeightCoef

        title: qsTr("Power")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[0]

        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
    }

    ColumnLayout {
        y: 160 * imageSlotHeightCoef + viewSettings.heightPairLed * 1.5 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            title: qsTr("Synch.")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            title: qsTr("Active")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[2]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }   // Column

    Controls.Led {
        y: 745 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Alarm")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[3]

        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
    }
}

