import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC
import QCC.Fonts 1.0

import Controls 1.0 as Controls

import Theme 1.0
import SSU 1.0


ListView {
    boundsBehavior: ListView.StopAtBounds
    flickableDirection: ListView.VerticalFlick

    currentIndex: -1
    clip: true

    readonly property var currentNetworkConfiguration: model === null ? null
                                                                      : currentIndex == -1 ? null
                                                                                           : model[currentIndex]

    delegate: ItemDelegate {
        id: itemDelegate
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: 0; bottomPadding: 0
        width: ListView.view.width
        height: Theme.rowHeight

        highlighted: ListView.isCurrentItem
        onClicked: {
            if(ListView.isCurrentItem) {
                ListView.view.currentIndex = -1
            }
            else {
                ListView.view.currentIndex = index
            }
        }

        text: qsTr("Configuration") + " " + (index + 1) + (ip4.length > 0 ? ": " + ip4
                                                                          : "")

        Material.background: Material.Blue

        contentItem: RowLayout {
            spacing: Theme.smallIndent

            Rectangle {
                width: Theme.pixelSize; height: width
                radius: 2
                color: isValid ? Material.color(Material.LightGreen)
                               : Material.color(Material.Grey)
            }

            Label {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                text: itemDelegate.text
                elide: Qt.ElideRight
            }

            Loader {
                visible: isValid && (itemDelegate.hovered || itemDelegate.ListView.isCurrentItem)
                Layout.alignment: Qt.AlignVCenter

                sourceComponent: Label {
                    font.pixelSize: Theme.pixelSize * 0.7
                    leftPadding: Theme.middleIndent; rightPadding: Theme.middleIndent
                    topPadding: Theme.smallIndent * 0.7; bottomPadding: Theme.smallIndent * 0.7

                    Material.foreground: "white"
                    background: Rectangle {
                        radius: 3
                        border.color: "white"
                        color: ntp.isValid ? Material.color(Material.LightBlue)
                                           : Material.color(Material.Grey)
                    }

                    text: qsTr("NTP")
                }
            }

            Loader {
                visible: isValid && (itemDelegate.hovered || itemDelegate.ListView.isCurrentItem)
                Layout.alignment: Qt.AlignVCenter

                sourceComponent: Label {
                    font.pixelSize: Theme.pixelSize * 0.7
                    leftPadding: Theme.middleIndent; rightPadding: Theme.middleIndent
                    topPadding: Theme.smallIndent * 0.7; bottomPadding: Theme.smallIndent * 0.7

                    Material.foreground: "white"
                    background: Rectangle {
                        radius: 3
                        border.color: "white"
                        color: ptp.isValid ? Material.color(Material.LightBlue)
                                           : Material.color(Material.Grey)
                    }

                    text: qsTr("PTP")
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                font.family: CFontLoader.awesome.name
                text: "\uf105"
            }
        }   // contentItem
    }   // ItemDelegate
}


