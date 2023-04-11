import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs
import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.Validators 1.0

Controls.StackPage {
    id: page

    property var device: null
    readonly property var card: device == null ? null
                                               : device.panel.inventory.monitoringCard

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()
    }

    Column {
        width: Math.min(parent.width, 30 * Theme.pixelSize)
        spacing: Theme.smallIndent

        QCC.TextField {
            id: ipField
            width: parent.width
            placeholderText: qsTr("IP address")
            readOnly: device == null ? true
                                     : !device.options.userRights.networkControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
            text: card === null ? "" :
                                  card.ip
        }

        QCC.TextField {
            id: ipMaskField
            //            visible: false
            width: parent.width
            placeholderText: qsTr("IP mask")   //Mac address
            readOnly: ipField.readOnly
            text: card === null ? ""
                                : card.mask
            validator: IpAddressValidator {}
        }

        QCC.TextField {
            id: portField
            width: parent.width
            placeholderText: qsTr("Port")
            readOnly: ipField.readOnly
            text: card === null ? "" :
                                  card.port
            validator: IntValidator {
                bottom: 0
            }
        }

        Row {
            anchors.right: parent.right
            spacing: Theme.middleIndent
            visible: !ipField.readOnly

            Button {
                text: qsTr("Cancel")
                onClicked: page.comeback()
            }

            Button {
                enabled: card === null ? false
                                       : ipField.text !== card.ip ||
                                         portField.text != card.port ||
                                         ipMaskField.text !== card.mask
                text: qsTr("Apply")
                onClicked: changeNetConfigPopup.open()
            }
        }   // Row
    }

    Dialogs.CheckPasswordPopup {
        id: changeNetConfigPopup
        title: qsTr("Are you sure?")
        text: qsTr("The connection will be dropped")
        password: device == null ? ""
                                 : device.connectionSettings.password
        applyButtonText: qsTr("Yes")
        onClicked: {
            device.send(card.commandToSetNetworkSettings(ipField.text, portField.text, ipMaskField.text))
            page.comeback()
        }
    }
}
