import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0

import QCC.Controls 1.0 as QCC

Popup {
    id: settingsPopup

    x: parent.width / 2 - width / 2
    leftMargin: Theme.indent; rightMargin: Theme.indent
    topMargin: Theme.indent; bottomMargin: Theme.indent

    property bool fillParentWidth: false

    width: fillParentWidth ? parent.width
                           : Math.max(Math.min(parent.width, parent.width * 0.4, Theme.pixelSize * 40), Theme.pixelSize * 20)

    modal: true
    clip: true
    property alias title: label.text
    property alias titleColor: label.color

    leftPadding: Theme.indent; rightPadding: Theme.indent
    topPadding: header.height// + Theme.smallIndent
    bottomPadding: Theme.smallIndent//Theme.indent

    background: Rectangle {
        radius: 3

        RowLayout {
            id: header
            anchors { left: parent.left; right: parent.right;
                leftMargin: Theme.indent; rightMargin: Theme.smallIndent; }

            Label {
                id: label
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                color: Material.accentColor
            }

            QCC.SymbolButton {
                symbol: "\uf00d"
                //                squareBackground: false
                onClicked: settingsPopup.close()
                topInset: 0; bottomInset: 0
            }
        }
    }
}
