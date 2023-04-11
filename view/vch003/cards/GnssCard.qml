import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Theme 1.0

MainCard {
    id: card
    source: "qrc:/images/vch003/GnssCard.jpg"

    Controls.Led {
        y: 40 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Power")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[0]

        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
    }

    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 140 * imageSlotHeightCoef
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        text: qsTr("Inputs")
    }

    ColumnLayout {
        y: 160 * imageSlotHeightCoef
        width: parent.width; height: 140 * imageSlotHeightCoef// viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            title: qsTr("Antenna")
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[1]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: "1 PPS"
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[2]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            title: "PTP"
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[3]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    Label {
        y: 315 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        text: qsTr("Tracking")
    }

    ColumnLayout {
        id: test
        y: 330 * imageSlotHeightCoef
        width: parent.width; height: 140 * imageSlotHeightCoef// viewSettings.heightPairLed * imageSlotHeightCoef
        spacing: 0

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            title: qsTr("Antenna")
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[4]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: "1 PPS"
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[5]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }

        Controls.Led {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            title: "PTP"
            spacing: 0
            obj: cardModel === null || cardModel.leds === undefined ? null
                                                                    : cardModel.leds.data[6]

            ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        }
    }

    ColumnLayout {
        y: 515 * imageSlotHeightCoef
        width: parent.width; height: 220 * imageSlotHeightCoef
        spacing: 0
        //        spacing: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)

        Label {
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            text: "1 PPS"
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: onePpsInput.width; implicitHeight: onePpsInput.height; radius: width / 2
            color: "#715F2D" //"#9C9A9D"

            Image {
                id: onePpsInput
                width: Math.min(imageSlotWidthCoef * 63, imageSlotHeightCoef * 63)
                source: "qrc:/images/input.png"
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                smooth: true
                mipmap: true
            }
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
            text: "10 MHz"
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: mhzInput.width; implicitHeight: mhzInput.height; radius: width / 2
            color: "#715F2D" //"#9C9A9D"

            Image {
                id: mhzInput
                width: Math.min(imageSlotWidthCoef * 63, imageSlotHeightCoef * 63)
                source: "qrc:/images/input.png"
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                smooth: true
                mipmap: true
            }
        }
    }



    Controls.Led {
        y: 745 * imageSlotHeightCoef
        anchors.horizontalCenter: parent.horizontalCenter

        title: qsTr("Alarm")
        obj: cardModel === null || cardModel.leds === undefined ? null
                                                                : cardModel.leds.data[7]

        font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
        ledSize: viewSettings.ledDiameter(imageSlotWidthCoef, imageSlotHeightCoef)
    }


    Column {
        y: 850 * imageSlotHeightCoef
        width: parent.width
        spacing: viewSettings.itemSpacing(imageSlotWidthCoef, imageSlotHeightCoef)

        Column {
            width: parent.width
            spacing: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
                text: "PTP"
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(imageSlotWidthCoef * 90, imageSlotHeightCoef * 90)
                source: "qrc:/images/Lan.jpg"
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                smooth: true
                mipmap: true
            }
        }

        Column {
            width: parent.width
            spacing: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: viewSettings.pixelSize(imageSlotWidthCoef, imageSlotHeightCoef)
                text: "NTP"
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(imageSlotWidthCoef * 90, imageSlotHeightCoef * 90)
                source: "qrc:/images/Lan.jpg"
                fillMode: Image.PreserveAspectFit
                antialiasing: true
                smooth: true
                mipmap: true
            }
        }
    }



}


