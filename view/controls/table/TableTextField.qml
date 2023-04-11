import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Theme 1.0

TextField {
    id: control
    verticalAlignment: Qt.AlignVCenter
    topInset: 0; bottomInset: 0
    leftInset: 0; rightInset: 0

    topPadding: 0; bottomPadding: 0
    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

    selectByMouse: true

    background: Item {
        Loader {
            width: parent.width; height: parent.height
            active: !control.acceptableInput && control.length > 0
            sourceComponent: Rectangle { color: Material.color(Material.Red, Material.Shade100); }
        }

        Loader {
            y: parent.height - height
            width: parent.width; height: 1
            active: control.activeFocus && !control.readOnly
            sourceComponent: Rectangle { color: Material.accentColor; }
        }
    }
}
