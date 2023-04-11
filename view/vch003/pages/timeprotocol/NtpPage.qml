import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import QCC.Controls 1.0 as QCC

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import Theme 1.0

GridLayout {
    id: ntpPage

    property int networkConfigurationIndex: -1
    property var ntpModel: null

    rowSpacing: Theme.smallIndent

    Rectangle {
        Layout.row: 0; Layout.column: 0
        Layout.columnSpan: 2
        Layout.fillWidth: true;
        implicitHeight: 1;
        color: Material.dropShadowColor;
    }

    Label {
        Layout.row: 1; Layout.column: 0
        leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
        text: qsTr("NTP")
        color: Material.accentColor
        font.bold: true
    }

    Loader {
        Layout.alignment: Qt.AlignTop | Qt.AlignRight
        Layout.row: 1; Layout.column: 1
        Layout.rowSpan: 2

        active: controlRights
        visible: active

        sourceComponent: QCC.LinkButton {
            text: qsTr("Remove")
            color: Material.color(Material.Red)

            onClicked: removeConfPopup.open()

            Dialogs.ConfirmationPopup {
                id: removeConfPopup
                text: qsTr("Are you sure you want to delete the NTP configuration?")
                onClicked: device.send(card.clearNtpConfigurationCommand(portModel, networkConfigurationIndex))
            }
        }
    }

    Controls.Group {
        Layout.row: 2; Layout.column: 0
        Layout.alignment: Qt.AlignTop
        topPadding: Theme.middleIndent

        onClicked: {
            if(ntpModel != null && controlRights) {
                controlPopup.openPopup(ntpConfigurationComponent, -1, qsTr("NTP"))
            }
        }

        contentItem: Grid {
            columns: 2
            spacing: Theme.middleIndent

            Label {
                text: qsTr("Stratum")
                font.bold: true
            }

            Label {
                text: ntpModel == null ? "-"
                                       : ntpModel.stratum
            }

            Label {
                text: qsTr("Server ref ID")
                font.bold: true
            }

            Label {
                text: ntpModel == null || ntpModel.serverRefId.length === 0 ? "-"
                                                                            : ntpModel.serverRefId
            }
        }
    }   // Group

    Component {
        id: ntpConfigurationComponent
        NtpConfiguration {
            ntpModel: ntpPage.ntpModel
            networkConfigurationIndex: ntpPage.networkConfigurationIndex
        }
    }
}   // GridLayout



