import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import SSU 1.0
import Theme 1.0

ListView {
    clip: true

    header: headerDelegate
    delegate: cardDelegate

    boundsBehavior: ListView.StopAtBounds
    flickableDirection: ListView.VerticalFlick
    headerPositioning: ListView.OverlayHeader

    Component {
        id: headerDelegate

        Controls.TableHeader {
            width: ListView.view.width
            Material.elevation: 1

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                text: qsTr("Slot")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                text: qsTr("Card")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                text: qsTr("Serial number")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                text: qsTr("Software")
                color: Material.accentColor
            }

            Controls.Separator { orientation: Qt.Vertical; }

            Controls.TableCell {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                text: qsTr("Hardware")
                color: Material.accentColor
            }
        }
    }

    Component {
        id: cardDelegate

        Loader {
            active: type !== SSU.Stub
            width: ListView.view.width
            height: Theme.rowHeight

            sourceComponent: Controls.RowLayoutPane {
                width: parent.width;
                seperatorActive: true
                spacing: 0

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                    text: slot
                }

                Controls.Separator { orientation: Qt.Vertical; }

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 3
                    text: SSU.cardTypeToString(type)
                }

                Controls.Separator { orientation: Qt.Vertical; }

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                    text: serialNumber
                }

                Controls.Separator { orientation: Qt.Vertical; }

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                    text: software
                }

                Controls.Separator { orientation: Qt.Vertical; }

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                    text: hardware
                }
            }
        }

    }
}
