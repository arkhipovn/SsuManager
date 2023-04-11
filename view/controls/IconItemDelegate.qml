import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Theme 1.0
import QCC.Fonts 1.0

ItemDelegate {
    id: item
    //    leftPadding: 0; rightPadding: 0;
    //    bottomPadding: 0; topPadding: 0
    padding: 0

    property alias symbol: symbolLabel.text
    property alias symbolScale: symbolLabel.scale
    property alias symbolColor: symbolLabel.color

    contentItem: RowLayout {
        Label {
            id: symbolLabel
            Layout.preferredWidth: Theme.pixelSize * 4
            Layout.fillHeight: true

            verticalAlignment: Label.AlignVCenter
            horizontalAlignment: Label.AlignHCenter

            text: "\uf0ac"
            font.family: CFontLoader.awesome.name
            font.pixelSize: Theme.pixelSize * 2
            color: Material.foreground
        }

        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            text: item.text
        }
    }
}
