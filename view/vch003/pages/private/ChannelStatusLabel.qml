import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0
import Controls 1.0 as Controls

Label {
    id: control

    width: Theme.textFieldWidth; height: Theme.rowHeight

    property alias backgroundColor: backgroundRec.color
    property alias hovered: mouseArea.containsMouse
    property alias hoverEnabled: mouseArea.hoverEnabled

    property string priority: ""

    property alias toolTip: toolTipItem

    property bool clickable: false

    signal clicked(var mouse)

    color: "white"
    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignHCenter

    elide: Qt.ElideRight

    leftPadding: Theme.middleIndent * 0.9
    rightPadding: Theme.middleIndent * 0.9

    background: Rectangle {
        id: controlBackground
        color: mouseArea.containsMouse ? Material.rippleColor
                                       : "transparent"

        Rectangle {
            id: backgroundRec

            readonly property int padding: 2
            x: padding; y: padding
            width: parent.width - padding * 2; height: parent.height - padding * 2

            radius: 3
            border.color: "white"
        }

        Loader {
            id: priorityLoader
            active: control.priority.length > 0
            sourceComponent: Label {
                topPadding: 3; bottomPadding: 3
                width: Theme.pixelSize * 1.4;
                x: controlBackground.width - width * 0.8; y: controlBackground.height - height * 0.8
                text: control.priority
                font.pixelSize: Theme.pixelSize * 0.7
                color: "white"

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                clip: true

                background: Rectangle {
                    radius: 4
                    border.width: 2
                    border.color: "white"
                    color: control.backgroundColor
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        enabled: clickable || hoverEnabled
        width: parent.width; height: parent.height
        hoverEnabled: true
        onClicked: {
            control.forceActiveFocus()
            if(control.clickable) control.clicked(mouse)
        }
    }

    Controls.ToolTip {
        id: toolTipItem
        visible: text.length > 0 && mouseArea.containsMouse
    }
}
