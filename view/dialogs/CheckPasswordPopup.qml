import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Theme 1.0

import QCC.Controls 1.0 as QCC

Loader {
    id: loader
    active: false
    onLoaded: item.open()

    width: Theme.pixelSize * 20
    x: parent.width / 2 - width / 2;

    function open() { active = true; }
    function close() { active = false; }

    property string title: ""
    property string text: ""
    property string password: ""
    property string applyButtonText: qsTr("Reset")

    signal clicked()
    signal canceled()

    sourceComponent: Popup {
        id: popup
        modal: true
        width: parent.width
        leftMargin: Theme.indent; rightMargin: Theme.indent
        topMargin: Theme.indent; bottomMargin: Theme.indent
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent
        bottomPadding: Theme.smallIndent

        onClosed: loader.close()

        contentItem: Column {
            spacing: Theme.middleIndent

            Label {
                id: titleLabel
                width: parent.width
                wrapMode: Label.WordWrap
                horizontalAlignment: Label.AlignHCenter
                color: Material.color(Material.Red)
                text: loader.title
            }

            Label {
                id: subTitleLabel
                visible: loader.text.length > 0
                width: parent.width
                wrapMode: Label.WordWrap
                horizontalAlignment: Label.AlignHCenter
                text: loader.text
            }

            QCC.PasswordTextField {
                id: passwordField
                width: parent.width
                incorrectInput: passwordField.text !== loader.password
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter;
                spacing: Theme.smallIndent;

                Button {
                    enabled: passwordField.acceptableInput
                    text: loader.applyButtonText
                    Material.background: Material.Red
                    Material.foreground: "white"
                    onClicked: {
                        popup.close()
                        loader.clicked()
                    }
                }

                Button {
                    text: qsTr("Cancel")
                    focus: true
                    onClicked: {
                        popup.close()
                        loader.canceled()
                    }
                }
            }
        }
    }   // Popup
}
