import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import QCC.Controls 1.0 as QCC

import Theme 1.0

import QtQuick.Layouts 1.12

Page {
    id: page
    property var device: null
    property int groupNumber: 0
    property var card: null

    topPadding: Theme.middleIndent; bottomPadding: Theme.smallIndent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    property alias footerData: footerRow.data

    footer: Controls.ToolBar {
        Material.elevation: 0

        contentItem: RowLayout {
            //        contentItem: Row {
            id: footerRow
            layoutDirection: Qt.RightToLeft
            spacing: 0
            width: parent.width

            QCC.SymbolButton {
                id: cardInfoButton
                topInset: 0; bottomInset: 0

                Layout.alignment: Qt.AlignVCenter
                //                anchors.verticalCenter: parent.verticalCenter
                squareBackground: false
                symbol: "\uf29c" //"\uf059"
                symbolScale: 1.1
                clickable: false
                toolTip.visible: cardInfoButton.hovered && card != null
                toolTip.contentItem: Column {
                    Label { visible: card.serialNumber !== undefined && card.serialNumber.length > 0; text: qsTr("Serial number") + ": " + card.serialNumber; }
                    Label { visible: card.software !== undefined && card.software.length > 0; text: qsTr("Software") + ": " + card.software; }
                    Label { visible: card.hardware !== undefined && card.hardware.length > 0; text: qsTr("Hardware") + ": " + card.hardware; }
                }
            }
        }
    }
}
