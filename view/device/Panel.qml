import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12
import Theme 1.0

Pane {
    id: panel

    property var device: null
    readonly property real slotWidth: device == null ? 0
                                                     : device.options.slotWidth(contentItem.width)
    property alias color: backgroundRec.color

    leftPadding: 1
    rightPadding: 1
    topPadding: addressRow.height + 2
    bottomPadding: 1


    background: Rectangle {
        id: backgroundRec
        color: "#7B7B7B"

        layer.enabled: panel.enabled && panel.Material.elevation > 0
        layer.effect: ElevationEffect {
            elevation: panel.Material.elevation
        }

        // header
        RowLayout {
            id: addressRow
            anchors { top: parent.top; left: parent.left; right: parent.right; margins: 1; }
            height: panel.slotWidth * 0.4
            spacing: 1

            Repeater {
                model: device == null ? null
                                      : device.options.kNumberOfSlots

                Rectangle {
                    Layout.fillWidth: true; Layout.fillHeight: true

                    color: viewSettings.panelColor
                    Label {
                        anchors.centerIn: parent
                        text: "A" + index.toString().padStart(2, "0")
                        font.pixelSize: panel.slotWidth * 0.25;
                    }
                }
            }   // address Repeater
        }   // RowLayout
    }
}

