pragma Singleton
import QtQuick 2.0
import QtQuick.Controls.Material 2.12
import SSU 1.0

QtObject {
    property real scale: 100

    //    readonly property real pixelSize: Qt.application.font.pixelSize * programSettings.scale / 100.
    readonly property real pixelSize: Qt.application.font.pixelSize * scale / 100.
    readonly property real indent: Qt.application.font.pixelSize * 1.5
    readonly property real middleIndent: indent * 0.61803
    readonly property real smallIndent: middleIndent * 0.61803

    readonly property real rowHeight: Math.max(pixelSize * 2.7, 30) //Math.max(pixelSize * 1.4, Material.buttonHeight)
    readonly property real textFieldWidth: Theme.pixelSize * 8
}




