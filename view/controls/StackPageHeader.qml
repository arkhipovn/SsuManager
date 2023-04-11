import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Theme 1.0
import QCC.Controls 1.0 as QCC

Pane {
    id: header

    topPadding: 0; bottomPadding: 0
    leftPadding: Theme.indent; rightPadding: Theme.indent

    Material.elevation: 1
    height: Theme.rowHeight * 2

    property alias title: label.text
    property alias backButtonVisible: backButton.visible

    default property alias content: row.data

    signal comeback()

    contentItem: Item {
        Label {
            id: label
            anchors { left: parent.left; right: row.left;
                rightMargin: Theme.smallIndent;
                verticalCenter: parent.verticalCenter; }
            elide: Qt.ElideRight
            color: Material.accentColor
        }

        RowLayout {
            id: row
            anchors { right: backButton.left; verticalCenter: parent.verticalCenter; }
            spacing: Theme.smallIndent
        }

        QCC.SymbolButton {
            id: backButton
            anchors { verticalCenter: parent.verticalCenter; right: parent.right; }
            color: Material.accent
            text: qsTr("Back")
            symbol: "\uf053"
            onClicked: header.comeback()
        }
    }
}
