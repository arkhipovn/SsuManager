import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0
import Controls 1.0 as Controls

//@N переделать бы

Controls.GroupBox {
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    property alias header: headerLoader.sourceComponent

    contentItem: Column {
        spacing: 0

        Loader {
            id: headerLoader
            width: parent.width
            active: sourceComponent !== null
            sourceComponent: null
        }

        Column {
            width: parent.width
            spacing: Theme.smallIndent

            Repeater {
                id: repeater
            }
        }
    }
}

