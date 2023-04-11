import QtQuick.Controls 2.12 as C
import QtQuick.Controls.Material 2.12

C.ToolTip {
    id: control
    Material.foreground: Material.primary
    delay: 300

    contentItem: C.Label {
        wrapMode: C.Label.WordWrap
        text: control.text
        font: control.font
        color: control.Material.foreground
    }
}
