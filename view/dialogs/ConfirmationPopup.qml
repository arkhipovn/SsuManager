import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Theme 1.0

Loader {
    id: loader
    active: false
    onLoaded: item.open()

    function open() { active = true; }
    function close() { active = false; }

    property string text: ""

    signal clicked()
    signal canceled()

    sourceComponent: Popup {
        id: popup
        modal: true
        leftMargin: Theme.indent; rightMargin: Theme.indent
        topMargin: Theme.indent; bottomMargin: Theme.indent
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent
        bottomPadding: Theme.smallIndent

        onClosed: loader.close()

        contentItem: Row {
            spacing: Theme.smallIndent

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: loader.text
                width: Theme.pixelSize * 13
                wrapMode: Label.WordWrap
                Material.foreground: Material.accent
            }

            Button {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Yes")
                onClicked: {
                    popup.close()
                    loader.clicked()
                }
            }

            Button {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Cancel")
                focus: true
                onClicked: {
                    popup.close()
                    loader.canceled()
                }
            }
        }
    }   // Popup
}
