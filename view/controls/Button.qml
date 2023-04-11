import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12
import Theme 1.0
import QCC.Controls 1.0 as QCC

QCC.SymbolButton {
    topInset: 0; bottomInset: 0

    symbolLabel.topPadding: Theme.smallIndent;
    symbolLabel.bottomPadding: Theme.smallIndent;

    symbolLabel.leftPadding: Theme.middleIndent * 0.9
    symbolLabel.rightPadding: Theme.middleIndent * 0.9
    size: Theme.pixelSize
    squareBackground: false
    scaleToContentSize: true

    property alias backgroundColor: backgroundRec.color
    property alias border: backgroundRec.border

    symbolLabel.background: Rectangle {
        id: backgroundRec
        radius: 3
        border.color: "white"
        color: Material.color(Material.LightBlue)
    }

    color: "white"
}
