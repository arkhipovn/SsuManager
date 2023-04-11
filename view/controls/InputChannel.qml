import QtQuick 2.12
import QtQuick.Controls 2.12
import Controls 1.0 as Controls

Item {
    id: input

    property alias color: backgroud.color
    property alias hoverEnabled: mouse.hoverEnabled
    property alias hovered: mouse.containsMouse

    property alias toolTip: inputToolTip

    signal clicked()

    Rectangle {
        id: backgroud
        anchors.centerIn: image
        width: image.paintedWidth; height: image.paintedHeight
        radius: width / 2
        scale: image.scale
        color: "#9C9A9D"
    }

    Image {
        id: image
        width: parent.width; height: parent.height
        source: "qrc:/images/input.png"
        fillMode: Image.PreserveAspectFit
        antialiasing: true
        smooth: true
        mipmap: true

        scale: input.hovered && mouse.containsMouse ? 1.08 : 1.0

        Behavior on scale {
            NumberAnimation { duration: 100; }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton
            hoverEnabled: true
            onClicked: input.clicked()
        }
    }

    Controls.ToolTip {
        id: inputToolTip
        visible: input.hovered && text.length > 0
    }
}



