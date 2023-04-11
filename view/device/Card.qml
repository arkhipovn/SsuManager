import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import SSU 1.0

Image {
    id: card
    smooth: true
    mipmap: true
    antialiasing: true

    property int numberOfSeats: 1

    //@N Что-то нужно сделать с panel... Тк сейчас зависит от id все..
    // Как вот в этом случае поступать?     slotWidth берется из Panel. А Panel - это контейнер для плат
    width: panel.slotWidth * numberOfSeats + (numberOfSeats - 1)

    property bool clickable: true
    signal clicked(var mouse)

    property alias hovered: mouse.containsMouse

    readonly property var cardModel: modelData

    readonly property real imageSlotWidthCoef: width / sourceSize.width
    readonly property real imageSlotHeightCoef: height / sourceSize.height

    property bool selected: false

    scale: clickable && mouse.pressed ? 1.02 :
                                        (mouse.containsMouse || selected ? 1.01
                                                                         : 1.0)

    Behavior on scale {
        NumberAnimation { duration: 100; }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.LeftButton
        hoverEnabled: true
        onClicked: card.clicked(mouse)
    }

    Controls.ToolTip {
        delay: 1000
        timeout: 2000
        visible: mouse.containsMouse
        contentItem: Column {
            Label { text: SSU.cardTypeToString(cardModel.type); color: Material.color(Material.Orange); }
            Label { visible: cardModel.serialNumber !== undefined && cardModel.serialNumber.length > 0; text: qsTr("Serial number") + ": " + cardModel.serialNumber; }
            Label { visible: cardModel.software !== undefined && cardModel.software.length > 0; text: qsTr("Software") + ": " + cardModel.software; }
            Label { visible: cardModel.hardware !== undefined && cardModel.hardware.length > 0; text: qsTr("Hardware") + ": " + cardModel.hardware; }
        }
    }
}


