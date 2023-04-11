import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QCC.Controls 1.0 as QCC

import Theme 1.0

// Тут мб не переопределять? А сделать вложенным MaskView

MaskView {
    id: maskDelegate
    property alias applyToAllButtonVisible: applyToAllButtonLoader.active

    signal applied(var name, var data)
    signal dataApplied(var data)
    signal nameApplied(var name)

    signal appliedToAll()

    Row {
        QCC.SymbolButton {
            id: applyButton
            text: qsTr("Apply")
            enabled: (maskNameChanged || maskDataChanged) && maskDelegate.name.length > 0 && !maskDelegate.incorrectMaskName
            // f00c

            onClicked: apply()

            function apply() {
                if(maskDelegate.model == null) {
                    maskDelegate.applied(maskDelegate.getName(), maskDelegate.getData())
                }
                else {
                    if(maskDelegate.maskNameChanged && maskDelegate.maskDataChanged)
                        maskDelegate.applied(maskDelegate.getName(), maskDelegate.getData())
                    else if(maskDelegate.maskDataChanged)
                        maskDelegate.dataApplied(maskDelegate.getData())
                    else if(maskDelegate.maskNameChanged)
                        maskDelegate.nameApplied(maskDelegate.getName())
                }
                maskDelegate.cancel()
            }
        }

        Loader {
            active: maskDelegate.model != null && (maskNameChanged || maskDataChanged)
            visible: active
            sourceComponent: QCC.SymbolButton {
                text: qsTr("Cancel")
                onClicked: maskDelegate.cancel()
            }
        }

        Loader {
            id:applyToAllButtonLoader
            active: false
            visible: active
            sourceComponent: QCC.SymbolButton {
                text: qsTr("Apply to all channels")
                onClicked: {
                    applyButton.apply()
                    maskDelegate.appliedToAll()
                }
            }
        }
    }
}
