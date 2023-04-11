import QtQuick 2.12
import QtQuick.Controls 2.12

import Models.Mask 1.0
import SSU 1.0

ComboBox {
    id: maskComboBox
    textRole: "name"

    function addSourceModel(model) {
        concatenateMaskListModel.addSourceModel(model)
    }

    signal currentMaskChanged(var mask)

    onActivated: {
        currentMaskChanged(concatenateMaskListModel.mask(currentIndex))
    }

    currentIndex: 0

    displayText: currentIndex == 0 ? qsTr("Select mask") :
                                     currentText

    model: ConcatenateMaskListModel {
        id: concatenateMaskListModel
    }

    onCountChanged: currentIndex = 0

    Connections {
        target: maskComboBox.model
        onDataChanged: {
            // Обработаем изменение масок..
            if(topLeft.row >= maskComboBox.currentIndex &&
                    bottomRight.row <= maskComboBox.currentIndex) {
                // Изменилась текущая маска.. Поменяем ее
                maskComboBox.currentMaskChanged(concatenateMaskListModel.mask(currentIndex))
            }
        }
    }
}
