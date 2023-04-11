import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Theme 1.0

import Controls 1.0 as Controls
import QCC.Controls 1.0 as QCC

RowLayout {
    id: row

    spacing: Theme.middleIndent

    property alias model: comboBox.model
    property alias displayText: comboBox.displayText
    property alias delegateTextFunc: comboBox.delegateTextFunc

    property alias currentIndex: comboBox.currentIndex
    property alias currentText: comboBox.currentText
    property alias value: comboBox.value

    ComboBox {
        Layout.preferredWidth: Theme.pixelSize * 6
        model: ["=", "!="]
        currentIndex: filterData == null ? 0
                                         : filterData.filterNegation ? 1
                                                                     : 0
        onActivated: {
            filterData.filterNegation = currentIndex == 0 ? false
                                                          : true
        }
    }

    Controls.ComboBox {
        id: comboBox
        Layout.fillWidth: true

        Binding {
            target: comboBox
            property: "value"
            value: filterData == null ? ""
                                      : filterData.filterValue
        }

        onActivated: filterData.filterValue = value
    }
}
