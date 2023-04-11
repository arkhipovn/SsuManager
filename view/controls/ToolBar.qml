import QtQuick.Controls 2.12 as C
import QtQuick.Controls.Material 2.12
import Theme 1.0

C.ToolBar {
    padding: 0
    //@N QML ToolBar (parent or ancestor of Material): Binding loop detected for property "foreground"
    leftPadding: Theme.middleIndent; rightPadding: Theme.middleIndent
}
