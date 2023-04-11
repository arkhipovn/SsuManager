import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0

import QCC.Fonts 1.0

Loader {
    id: infoChangesLoader
    width: Theme.pixelSize * 1.4; height: width
    x: - width / 5; y: - height / 5
    active: false

    property string symbol: "A"

    sourceComponent: Label {
        text: infoChangesLoader.symbol
        font.pixelSize: Theme.pixelSize * 0.7
        color: "white"

        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        clip: true

        background: Rectangle {
            radius: height * 0.5
            border.width: 2
            border.color: "white"
            color: Material.color(Material.Blue)
        }
    }



    //    sourceComponent: Rectangle {
    //        radius: height * 0.5
    //        color: Material.color(Material.Blue)
    //        Label {
    //            anchors.centerIn: parent
    //            text: infoChangesLoader.symbol
    //            font.pixelSize: Theme.pixelSize * 0.7
    //            color: "white"
    //        }
    //    }
}

