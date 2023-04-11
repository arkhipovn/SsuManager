import QtQuick 2.12
import QtQuick.Controls 2.12
import Theme 1.0

import Qt.labs.settings 1.0
import SSU 1.0

ApplicationWindow {
    id: window
    visible: true
    font.capitalization: Font.MixedCase
    font.pixelSize: Theme.pixelSize

    property alias settings: settingsObject
    property alias settingsCategory: settingsObject.category

    Settings {
        id: settingsObject
        fileName: SSU.programSettingsFile()
        //        property alias x: window.x
        //        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }
}
