import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import Theme 1.0

import Controls 1.0 as Controls

Label {
    id: control
    elide: Qt.ElideRight
    //    height: Theme.rowHeight

    horizontalAlignment: Label.AlignHCenter
    verticalAlignment: Label.AlignVCenter

    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

    property alias toolTip: toolTipItem

    property bool clickable: false
    property alias hovered: mouse.containsMouse
    property bool active: true

    property alias hoverEnabled: mouse.hoverEnabled

    signal clicked()

    background: Ripple {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: parent.width; height: parent.height

        clipRadius: 2

        pressed: mouse.pressed && control.clickable
        anchor: control
        active: control.active
        color: control.hovered && clickable ? "#E6E6E6"
                                            :  Material.rippleColor
    }

    MouseArea {
        id: mouse
        width: parent.width; height: parent.height
        hoverEnabled: true
        onClicked: if(control.clickable) control.clicked()
    }

    Controls.ToolTip {
        id: toolTipItem
        visible: text.length > 0 && control.hovered && clickable
    }
}

