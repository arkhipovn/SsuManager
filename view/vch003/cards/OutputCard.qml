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
        id: clkLedColumn
        y: 160 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            title: qsTr("Clk A06")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            title: qsTr("Clk A08")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[2]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    ColumnLayout {
        y: clkLedColumn.y + viewSettings.heightPairLed * 1.5 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            title: qsTr("Synch.")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[3]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom

            title: qsTr("Active")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[4]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }   // Column

    ColumnLayout {
        y: 745 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            title: qsTr("Alarm")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[5]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            title: qsTr("Bypass")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[6]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

}

