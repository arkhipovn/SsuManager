import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import SSU 1.0

import Controls 1.0 as Controls
import Theme 1.0

Item {
    id: skyView
    property alias model: repeater.model

    property var labelFontSize: 10
    property var margins: labelFontSize * 8

    property var selectedRows: []

    Image {
        id: systemImage

        x: parent.width * 0.5 - width * 0.5
        y: parent.height * 0.5 - height * 0.5

        width: Math.min(skyView.width, skyView.height) - skyView.margins
        height: width

        readonly property real widthCoef: width / sourceSize.width
        readonly property real heightCoef: height / sourceSize.height

        source: "qrc:/images/polar.png"
        fillMode: Image.PreserveAspectFit
        antialiasing: true

        property int highlightedIndex: -1
    }

    Repeater {
        model: [ 90, 75, 60, 45, 30, 0 ]

        delegate: Label {
            x: systemImage.widthCoef * 4 + // отступ
               systemImage.x + systemImage.width / 2
            y: systemImage.heightCoef * 6 +  // отступ
               systemImage.y + systemImage.height * 0.5 * (1 - Math.sin((90 - modelData) * Math.PI / 180))

            text: modelData + "°"
            font.pixelSize: skyView.labelFontSize
            font.weight: Font.ExtraLight
        }
    }   // Repeater

    Repeater {
        model: [ 0, 45, 90, 135, 180, 225, 270, 315 ]
        delegate: Label {
            x: systemImage.x + systemImage.width * 0.5 +    // центр
               (systemImage.width * 0.5 + width * 0.5) * Math.cos((modelData - 90) * Math.PI / 180)
               - width * 0.5

            y: systemImage.y + systemImage.height * 0.5 +    // центр
               (systemImage.width * 0.5 + height * 0.5) * Math.sin((modelData - 90) * Math.PI / 180)
               - height * 0.5

            text: {
                let str =  modelData + "°"
                switch(modelData) {
                case 0:   return SSU.hemisphereToString(SSU.North)  + " " + str
                case 90:  return SSU.hemisphereToString(SSU.East)   + " " + str
                case 180: return SSU.hemisphereToString(SSU.South)  + " " + str
                case 270: return SSU.hemisphereToString(SSU.West)   + " " + str
                default: return str
                }
            }

            font.pixelSize: skyView.labelFontSize
            font.weight: Font.ExtraLight
        }
    }   // Repeater

    Repeater {
        id: repeater

        delegate: Rectangle {
            border { width: 1; color: "#4F4F4F"; }
            color: {
                if(!tracked)
                    return Material.color(Material.Grey)
                switch(type) {
                case SSU.GPS: return Material.color(Material.Blue)
                case SSU.GLONASS: return Material.color(Material.Green)
                case SSU.Galileo: return Material.color(Material.Teal)
                case SSU.BeiDou: return Material.color(Material.Red)
                }
                return Material.accentColor
            }

            width: mouseArea.containsMouse || selectedRows.includes(index) ? 17
                                                                           : 12
            height: width
            radius: width * 0.5

            x: systemImage.x + systemImage.width * 0.5 +    // центр
               (systemImage.width * 0.5 - systemImage.widthCoef * 5) * (Math.sin((90 - elevation) * Math.PI / 180) * Math.cos((azimuth - 90)* Math.PI / 180)) -
               width * 0.5
            y: systemImage.y + systemImage.height * 0.5 +    // центр
               (systemImage.height * 0.5 - systemImage.heightCoef * 5) * (Math.sin((90 - elevation) * Math.PI / 180) * Math.sin((azimuth - 90) * Math.PI / 180)) -
               height * 0.5

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
            }

            Controls.ToolTip {
                width: Theme.indent * 12
                visible: mouseArea.containsMouse
                text: SSU.satelliteSystemToString(type) + " " + satId + "\r\n" +
                      (tracked ? qsTr("Tracked") : qsTr("Not Tracked")) +"\r\n\r\n" +
                      qsTr("Elevation") + ": " + elevation + " °\r\n" +
                      qsTr("Azimuth") + ": " + azimuth + " °\r\n" +
                      qsTr("SNR") + ": " + snr + " " + qsTr("dB-Hz")
            }
        }   // Rec
    }   // Repeater
}


