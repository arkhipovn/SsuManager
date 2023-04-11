import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0

import QCC.Fonts 1.0
import QCC.Controls 1.0 as QCC

QCC.SymbolButton {
    topInset: 0; bottomInset: 0
    squareBackground: false

    font.pixelSize: Theme.pixelSize * 1.5

    property alias infoChanges: infoChangesLoader.active

    Loader {
        id: infoChangesLoader
        width: Theme.pixelSize; height: width
        y: parent.height - height
        x: parent.width - width

        active: false

        sourceComponent: Rectangle {
            radius: height * 0.5
            color: Material.color(Material.Orange)

            Label {
                anchors.centerIn: parent
                text: "\uf129" //"\uf05a"
                font.family: CFontLoader.awesome.name
                font.pixelSize: Theme.pixelSize * 0.7
                color: "white"//Material.color(Material.Orange)
            }
        }
    }
}
