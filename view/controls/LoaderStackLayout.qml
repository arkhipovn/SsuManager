import QtQuick 2.12
import QtQuick.Layouts 1.12

// Использователь если все элементы стека будет Loader
// Загружатся данные будут по мере использования

StackLayout {
    id: pageLoader

    /*readonly*/ property var currentItem: itemAt(currentIndex)

    onCurrentItemChanged: {
        if(currentItem !== null) {
            currentItem.active = true
        }
    }

    Component.onCompleted: {
        let i = itemAt(currentIndex)
        if(i !== null)
            i.active = true
    }
}
