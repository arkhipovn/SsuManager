import QtQuick 2.12
import QtQuick.Controls 2.12

import Device 1.0 as D
import Theme 1.0

D.Card {
    id: card
    source: {
        switch(numberOfSeats) {
        case 2: return "qrc:/images/vch003/MiddleCard.jpg"
        case 1:
        default: return "qrc:/images/vch003/SmallCard.jpg"
        }
    }

    default property alias contentItem: content.data
    property string title: viewSettings.cardTitle(cardModel.type)

    //Rect 3, 22, 46, 31
    Label {
        id: topTitle
        x: 4 * imageSlotWidthCoef; y: 50 * imageSlotHeightCoef; z: 2
        width: 110 * imageSlotWidthCoef; height: 76 * imageSlotHeightCoef
        text: card.title
        font { pixelSize: viewSettings.bigPixelSize(imageSlotWidthCoef, imageSlotHeightCoef); weight: Font.DemiBold; }
        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
    }

    //Rect 3, 592, 46, 31
    Label {
        id: bottomTitle
        x: 4 * imageSlotWidthCoef; y: 1422 * imageSlotHeightCoef; z: 2
        width: 110 * imageSlotWidthCoef; height: 76 * imageSlotHeightCoef
        text: card.title
        font { pixelSize: viewSettings.bigPixelSize(imageSlotWidthCoef, imageSlotHeightCoef); weight: Font.DemiBold; }
        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
    }

    Item {
        id: content
        anchors { top: topTitle.bottom; bottom: bottomTitle.top; topMargin: 10 * imageSlotHeightCoef; bottomMargin: 80 * imageSlotHeightCoef; }
        //@N Что-то нужно сделать с panel... Тк сейчас зависит от id все..
        width: panel.slotWidth //parent.width / numberOfSeats - (numberOfSeats - 1)
    }
}


