import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Theme 1.0

ColumnLayout {
    id: ledItem

    spacing: ledSize * 0.2
    layoutDirection: Qt.RightToLeft

    property alias title: ledLabel.text

    property alias font: ledLabel.font
    property alias titleColor: ledLabel.color
    property alias ledSize: ledRec.implicitWidth

    property var obj: null
    property bool power: obj == null ? false : obj.power
    property color color: obj == null ? "green" : obj.color
    property bool blink: obj == null ? false : obj.blink

    property color ledOffColor: "#999999"

    onBlinkChanged: {
        blinkTimer.ledOn = true
        if(blink) blinkTimer.start()
        else blinkTimer.stop()
    }

    Timer {
        id: blinkTimer
        interval: ledOn ? 1000
                        : obj == null ? 1000 : obj.blinkInterval
        property bool ledOn: true
        onTriggered: ledOn = !ledOn
        repeat: true
    }

    Label {
        id: ledLabel
        Layout.fillWidth: true
        wrapMode: Label.WordWrap
        horizontalAlignment: Qt.AlignHCenter
        //        style: Text.Raised
        //        styleColor: Qt.rgba(0,0,0,0.5)
        //        font.weight: Font.Light
    }

    Rectangle {
        id: ledRec

        implicitWidth: 10; implicitHeight: implicitWidth
        Layout.alignment: Qt.AlignHCenter
        //        Layout.preferredWidth: 10; Layout.preferredHeight: Layout.preferredWidth
        //        width: 10; height: width;

        radius: width / 2
        //        anchors.horizontalCenter: parent.horizontalCenter
        color: (ledItem.power && blinkTimer.ledOn) ? ledItem.color : ledItem.ledOffColor

        Behavior on color {
            ColorAnimation { duration: 200; }
        }
    }
}

