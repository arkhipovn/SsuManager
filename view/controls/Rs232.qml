import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Theme 1.0
import Controls 1.0 as Controls

Item {
    id: rs232

    property alias title: label.text
    property int orientation: Qt.Vertical

    property color color: Material.color(Material.Grey)// "white"
    property alias color1: topBackgroud.color
    property alias color2: bottomBackgroud.color

    property alias hoverEnabled: mouse.hoverEnabled
    property alias hovered: mouse.containsMouse

    property alias font: label.font
    property alias titleColor: label.color

    property alias toolTip: rsToolTip

    Item {
        anchors.centerIn: image
        width: image.paintedWidth * 0.7; height: image.paintedHeight * 0.7
        scale: image.scale

        Rectangle {
            id: topBackgroud
            anchors { top: parent.top; bottom: bottomBackgroud.top; }
            width: parent.width
            color: rs232.color
        }

        Rectangle {
            id: bottomBackgroud
            width: parent.width; height: parent.height / 2
            anchors { bottom: parent.bottom; }
            color: rs232.color
        }
    }

    Image {
        id: image
        anchors { top: parent.top; bottom: label.top; bottomMargin: rs232.font.pixelSize * 0.5; }
        width: parent.width
        source: rs232.orientation == Qt.Vertical ? "qrc:/images/rs232.png"
                                                 : "qrc:/images/rs232_horizontal.png"
        fillMode: Image.PreserveAspectFit
        antialiasing: true
        smooth: true
        mipmap: true

        scale: rs232.hovered && mouse.containsMouse ? 1.03 : 1.0

        Behavior on scale {
            NumberAnimation { duration: 100; }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            hoverEnabled: true
        }
    }

    Label {
        id: label
        visible: text.length > 0
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom }
    }

    Controls.ToolTip {
        id: rsToolTip
        visible: rs232.hovered && text.length > 0
    }
}



