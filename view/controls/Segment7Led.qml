import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    border.width: 1; radius: 2
    border.color: "#454545"

    property int numberOfDigits: 1
    property bool dotAtEnd: true
    property bool highlightDotAtEnd: true

    property alias value: label.value
    property alias valueColor: label.color
    property alias offIndicatorColor: backLabel.color

    Label {
        id: backLabel
        width: parent.width; height: parent.height

        text: '8'.repeat(numberOfDigits) + (dotAtEnd && numberOfDigits > 0 ? "."
                                                                           : "")
        color: "grey"

        padding: label.padding

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        font: label.font
    }

    Label {
        id: label
        width: parent.width; height: parent.height

        FontLoader {
            id: fontLoader
            source: "qrc:/fonts/DSEG7Classic-Italic.ttf"
        }

        font.family: fontLoader.name
        font.pixelSize: Math.min(width, height) / numberOfDigits

        color: "indigo"

        padding: Math.min(width, height) * 0.05

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        property string value: ""

        text: value.length == 0 ? ""
                                : (value.length > numberOfDigits ? value.slice(0, numberOfDigits)
                                                                 : value.padStart(numberOfDigits, "0"))
                                  + (dotAtEnd && highlightDotAtEnd && value.length > 0 ? "."
                                                                                       : "")
    }
}
