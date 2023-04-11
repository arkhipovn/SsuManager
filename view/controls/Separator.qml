import QtQuick 2.0
import QtQuick.Controls.Material 2.12

Rectangle {
    z: 3
    property int orientation: Qt.Horizontal

    implicitWidth: orientation == Qt.Horizontal ? parent.width : 1
    implicitHeight: orientation == Qt.Horizontal ? 1 : parent.height
    color: Material.dropShadowColor;
}
