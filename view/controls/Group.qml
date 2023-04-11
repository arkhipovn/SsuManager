import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Theme 1.0

//@N Без понятия как лучше назвать..
import QCC.Controls 1.0 as QCC

GroupBox {
    id: control
    hoverEnabled: true

    signal clicked()

    background: Rectangle {
        color: control.hovered ? Material.highlightedRippleColor : "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: control.clicked()
        }
    }

    topPadding: Theme.middleIndent * 2 + Theme.smallIndent * 2
    leftPadding: Theme.smallIndent
    rightPadding: Theme.smallIndent
    bottomPadding: Theme.middleIndent

    label: Label {
        width: control.availableWidth
        elide: Label.ElideRight
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent
        leftPadding: control.leftPadding; rightPadding: control.rightPadding
        text: control.title
        color: Material.accentColor
        font.bold: true
    }
}


