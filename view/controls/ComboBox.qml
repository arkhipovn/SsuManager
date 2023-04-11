import QtQuick 2.12
import QtQuick.Controls 2.10 as C
import QtQuick.Controls.Material 2.12
import Theme 1.0

//@N что-то придумать с привязками.. теряется привязка у value из-за Binding

C.ComboBox {
    id: control
    property var delegateTextFunc: null

    displayText: model === null ? ""
                                : (delegateTextFunc == null ? (model.length === undefined ? currentIndex
                                                                                          : model[currentIndex])
                                                            : (model.length === undefined ? delegateTextFunc(currentIndex)
                                                                                          : delegateTextFunc(model[currentIndex])))

    property var value: 0

    currentIndex: model === null ? 0
                                 : model.length === undefined ? value
                                                              : model.indexOf(value)

    Binding {
        target: control
        property: "value"
        value: model === null ? currentIndex
                              : model.length === undefined ? currentIndex
                                                           : model[currentIndex]
    }

    delegate: C.ItemDelegate {
        width: parent.width; height: control.height
        topInset: 0; bottomInset: 0
        text: delegateTextFunc == null ? modelData
                                       : delegateTextFunc(modelData)
        Material.foreground: control.currentIndex === index ? control.Material.accent : control.Material.foreground

        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }
}


