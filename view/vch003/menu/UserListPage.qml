import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Menu 1.0 as M
import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC
import Theme 1.0
import SSU 1.0

M.MenuPage {
    //    Controls.ToolTip {
    //        id: errorToolTip
    //        text: qsTr("User not added")
    //        Material.background: Material.color(Material.Red, Material.Shade700)
    //        visible: false
    //        delay: 0
    //        timeout: 2000
    //    }   // ToolTip

    ListView {
        id: listView
        width: parent.width; height: parent.height
        clip: true

        model: device == null ? null
                              : device.userList

        header: headerDelegate
        delegate: userDelegate

        boundsBehavior: ListView.StopAtBounds
        flickableDirection: ListView.VerticalFlick
        headerPositioning: ListView.OverlayHeader
    }

    Component {
        id: headerDelegate

        Controls.TableHeader {
            width: listView.width; height: Theme.rowHeight + Theme.smallIndent
            Material.elevation: 1

            Controls.TableCell {
                Layout.preferredWidth: parent.width * 0.3
                Layout.fillHeight: true
                text: qsTr("User level")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true
                text: qsTr("Username")
                color: Material.accentColor
            }
        }
    }

    Component {
        id: userDelegate

        Controls.RowLayoutPane {
            id: userRow
            width: ListView.view.width; height: Theme.rowHeight + Theme.smallIndent
            hoverEnabled: true
            seperatorActive: true
            spacing: 0

            property bool userBlocked: untilTime > device.currentDateTime.getTime()
            readonly property bool userActive: device.connectionSettings.username === name

            backgroundColor: userBlocked ? "#FFDBDB"
                                         : userRow.hovered ? userRow.color : "white"

            Controls.ToolTip {
                visible: userBlocked && userRow.hovered
                text: qsTr("User blocked until") + " " + Qt.formatDateTime(untilTime, programSettings.dateTimeFormat)
            }

            Controls.TableCell {
                Layout.preferredWidth: parent.width * 0.3
                Layout.fillHeight: true;
                text: SSU.userLevelToString(level)
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true
                text: name
            }

            Label {
                Layout.alignment: Qt.AlignRight
                padding: Theme.middleIndent
                width: visible ? implicitWidth : 0
                visible: userRow.userActive || connected
                color: Material.color(Material.LightGreen)
                text: userRow.userActive ? qsTr("Current")
                                         : connected ? qsTr("Connected")
                                                     : ""
            }

            QCC.SymbolButton {
                id: userMenuButton
                topInset: 0; bottomInset: 0

                opacity: userRow.hovered || userMenu.opened ? 1.0 : 0.0
                visible: opacity != 0
                Behavior on opacity { NumberAnimation { duration: 200 } }
                color: Material.accentColor
                symbol: "\uf142"
                toolTip.text: qsTr("Menu")

                onClicked: userMenu.open()

                Popup {
                    id: userMenu
                    leftMargin: Theme.middleIndent; rightMargin: Theme.middleIndent
                    y: userMenuButton.height
                    spacing: 0
                    topPadding: 0; bottomPadding: 0
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

                    contentItem: Row {
                        QCC.SymbolButton {
                            id: delButton
                            visible: !userRow.userActive

                            color: Material.color(Material.Red)
                            symbol: "\uf014"
                            symbolScale: 1.1
                            toolTip.text: qsTr("Delete user")

                            onClicked: removeUserPopup.open()

                            Dialogs.ConfirmationPopup {
                                id: removeUserPopup
                                text: qsTr("Do you want to remove user") + " <b>" + name + "</b>?"
                                onClicked: device.deleteUser(name)
                            }
                        }

                        QCC.SymbolButton {
                            id: passwordChangeButton
                            color: Material.color(Material.Teal)
                            symbol: "\uf084"
                            toolTip.text: qsTr("Change password")
                            onClicked: controlPopup.openPopup(passwordSettingsComponent, qsTr("Change password") + " " + name)

                            Component {
                                id: passwordSettingsComponent

                                Column {
                                    QCC.PasswordTextField {
                                        id: passwordField
                                        width: parent.width

                                        placeholderText: qsTr("New password")

                                        property bool passwordRating: device.options.checkPassword(text)
                                        incorrectInput: !passwordRating
                                        message: text.length === 0 ? ""
                                                                   : passwordRating ? qsTr("Strong password")
                                                                                    : qsTr("The longer your password is, the harder it is to crack.
The password must consist of numbers, uppercase and lowercase letters, and punctuation")

                                        messageColor: passwordRating ? Material.color(Material.LightGreen, Material.Shade400)
                                                                     : Material.color(Material.Green, Material.ShadeA700)
                                    }

                                    Button {
                                        anchors.right: parent.right
                                        enabled: passwordField.acceptableInput
                                        text: qsTr("Apply")
                                        onClicked: {
                                            device.changeUserPassword(name, passwordField.text)
                                            controlPopup.close()
                                        }
                                    }
                                }
                            }   // passwordSettingsComponent
                        } // password button


                        QCC.SymbolButton {
                            id: unblockButton
                            visible: userRow.userBlocked
                            symbolScale: 1.2
                            color: Material.color(Material.LightGreen)
                            symbol: "\uf13e"
                            toolTip.text: qsTr("Unblock user")
                            onClicked: unblockUserPopup.open()

                            Dialogs.ConfirmationPopup {
                                id: unblockUserPopup
                                text: qsTr("Unblock user") + " <b>" + name + "</b>?"
                                onClicked: device.unblockUser(name)
                            }
                        }

                        QCC.SymbolButton {
                            id: blockButton
                            visible: !userRow.userActive
                            symbolScale: 1.2

                            color: Material.color(Material.Orange)
                            symbol: "\uf023"
                            toolTip.text: qsTr("Block user")

                            onClicked: controlPopup.openPopup(blockUserComponent, qsTr("Block user") + " " + name)

                            Component {
                                id: blockUserComponent

                                Column {
                                    QCC.DateInput {
                                        id: dateInput
                                        width: parent.width
                                        placeholderText: qsTr("Block until..")
                                        dateTime: userRow.userBlocked ? untilTime
                                                                      : programSettings.getCurrentDateTime(86400)
                                        format: programSettings.dateTimeFormat
                                    }

                                    Button {
                                        anchors.right: parent.right
                                        enabled: dateInput.acceptableInput
                                        text: qsTr("Apply")
                                        onClicked: {
                                            device.blockUser(name, programSettings.dateTimeFromString(dateInput.value))
                                            controlPopup.close()
                                        }
                                    }
                                }
                            }   // blockUserComponent
                        }   // blockUserButton

                        //                        QCC.SymbolButton {
                        //                            id: signoutButton
                        //                            visible: false// !userRow.userActive && connected
                        //                            symbolScale: 1.2
                        //                            color: Material.color(Material.LightBlue)
                        //                            symbol: "\uf08b"
                        //                            toolTip.text: qsTr("Sign out")

                        //                            Dialogs.ConfirmationPopup {
                        //                                id: signoutUserPopup
                        //                                text: qsTr("Sign out user") + " <b>" + name + "</b>?"
                        //                                //                                onClicked:  //@N add command
                        //                            }
                        //                        }   // Signout button

                    }   // Row
                }   // Popup menu
            }
        }   // RowLayoutPane
    }

    Dialogs.SettingsPopup {
        id: controlPopup
        y: Theme.indent
        width: Math.min(Theme.pixelSize * 40, parent.width * 0.5)

        function openPopup(component, title) {
            controlPopup.title = title
            loader.sourceComponent = component
            open()
        }

        onClosed: loader.sourceComponent = null

        contentItem: Loader {
            id: loader
            sourceComponent: null
        }
    }

    Component {
        id: addUserComponent

        Column {

            Controls.ComboBox {
                id: levelBox
                width: parent.width
                model: [ SSU.User, SSU.Admin, SSU.Security ]
                delegateTextFunc: SSU.userLevelToString
            }

            QCC.TextField {
                id: usernameField
                width: parent.width
                placeholderText: qsTr("Username")
            }

            QCC.PasswordTextField {
                id: passwordField
                width: parent.width

                placeholderText: qsTr("Password")

                property bool passwordRating: device == null ? false
                                                             : device.options.checkPassword(text)
                incorrectInput: !passwordRating
                message: text.length === 0 ? ""
                                           : passwordRating ? qsTr("Strong password")
                                                            : qsTr("The longer your password is, the harder it is to crack.
The password must consist of numbers, uppercase and lowercase letters, and punctuation")

                messageColor: passwordRating ? Material.color(Material.LightGreen, Material.Shade400)
                                             : Material.color(Material.Green, Material.ShadeA700)
            }

            QCC.PasswordTextField {
                id: passwordCopyField
                width: parent.width
                placeholderText: qsTr("Password confirmation")
                incorrectInput: passwordField.text != text
                message: text.length === 0 || !incorrectInput ? ""
                                                              : qsTr("Passwords do not match")
            }

            Button {
                anchors.right: parent.right
                enabled: passwordField.acceptableInput && passwordCopyField.acceptableInput
                text: qsTr("Apply")
                onClicked: {
                    device.addUser(usernameField.text, passwordField.text, levelBox.value)
                    controlPopup.close()
                }
            }
        }
    }

    footer: Controls.ToolBar {
        Row {
            anchors.verticalCenter: parent.verticalCenter
            visible: listView.model !== null

            QCC.SymbolButton {
                anchors.verticalCenter: parent.verticalCenter
                symbol: "\uf0c7" // floppy-o
                toolTip.text: qsTr("Save to csv")
                color: Material.accentColor
                onClicked: device.userList.saveInventoryInformation()
            }

            QCC.SymbolButton {
                anchors.verticalCenter: parent.verticalCenter
                symbol: "\uf02f"    // print
                toolTip.text: qsTr("Print")
                color: Material.accentColor
                onClicked: device.userList.printInventoryInformation()
            }
        }

        Row {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            QCC.SymbolButton {
                Layout.leftMargin: Theme.indent
                text: qsTr("Update")
                symbol: "\uf021"
                onClicked: device.updateUserList()
            }

            QCC.SymbolButton {
                Layout.leftMargin: Theme.indent
                text: qsTr("Add user")
                symbol: "\uf067"
                onClicked: controlPopup.openPopup(addUserComponent, qsTr("New user"))
            }
        }
    }
}
