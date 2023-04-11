import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Theme 1.0

MainCard {
    source: "qrc:/images/vch003/MonitoringCard.jpg"

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

    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 140 * imageSlotHeightCoef
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        text: "Alarm"
    }

    ColumnLayout {
        y: 160 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * 2 * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            title: "Minor" //qsTr("Minor")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]
            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: "Major" //qsTr("Major")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[2]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: "Critical" //qsTr("Critical")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[3]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            title: "Warning" //"Not alarms"//qsTr("Residual")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[4]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    ColumnLayout {
        id: test
        y: 745 * imageSlotHeightCoef
        width: parent.width; height: viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            title: qsTr("Power A")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[5]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: qsTr("Power B")
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[6]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }   // Column



}
