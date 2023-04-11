import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Theme 1.0

import QCC.Controls 1.0 as QCC
import ConnectionSettings 1.0

MenuPage {
    id: page

    bottomPadding: Theme.indent

    StackView {
        id: stackView
        width: parent.width; height: parent.height
        initialItem: programSettingsMenuComponent

        Connections {
            target: stackView.depth == 1 ? null
                                         : stackView.currentItem
            onComeback: stackView.pop()
        }
    }

    Component {
        id: programSettingsMenuComponent

        Controls.StackPage {
            leftPadding: 0; rightPadding: 0

            header: Controls.StackPageHeader {
                leftPadding: 0; rightPadding: 0
                contentItem: Label {
                    verticalAlignment: Label.AlignVCenter
                    leftPadding: Theme.indent; rightPadding: Theme.indent
                    elide: Qt.ElideRight
                    color: Material.accentColor
                    text: qsTr("Program settings")
                }
            }

            Pane {
                width: parent.width
                Material.elevation: 1
                padding: 0

                contentItem: Column {
                    Controls.IconItemDelegate {
                        text: qsTr("General")
                        width: parent.width
                        symbol: "\uf1de"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Teal)
                        onClicked: stackView.push("qrc:/Menu/GeneralSettingsStackPage.qml", { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Program data update settings")
                        width: parent.width
                        symbol: "\uf0e2"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Teal)
                        onClicked: stackView.push("qrc:/Menu/UpdateDataSettingsStackPage.qml", { title: text, device: page.device })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Alarm color indication")
                        width: parent.width
                        symbol: "\uf1fc"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Green)
                        onClicked: stackView.push("qrc:/Menu/EventDisplaySettingsStackPage.qml", { title: text })
                    }

                    Controls.IconItemDelegate {
                        text: qsTr("Program data management")
                        width: parent.width
                        symbol: "\uf1c0"
                        symbolScale: 0.9
                        symbolColor: Material.color(Material.Teal)
                        onClicked: stackView.push("qrc:/Menu/ProgramDataManagementStackPage.qml", { title: text, device: page.device })
                    }
                }
            }
        }
    }   // deviceSettingsMenuComponent
}
