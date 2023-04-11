import QtQuick 2.12
import QtQuick.Controls 2.12
import QCC.Controls 1.0 as QCC
import Theme 1.0

Column {
    property var ntpModel: null
    property int networkConfigurationIndex: -1

    Grid {
        width: parent.width
        verticalItemAlignment: Qt.AlignVCenter

        columns: 2
        spacing: Theme.middleIndent

        Label {
            text: qsTr("Stratum")
            font.bold: true
        }

        ComboBox {
            id: comboBox
            model: 16
            currentIndex: ntpModel == null ? 0
                                           : ntpModel.stratum
        }

        Label {
            text: qsTr("Server ref ID")
            font.bold: true
        }

        TextField {
            id: serverRefIdField
            text: ntpModel == null || ntpModel.serverRefId.length === 0 ? ""
                                                                        : ntpModel.serverRefId
        }
    }

    Button {
        enabled: ntpModel == null ||
                 comboBox.currentIndex !== ntpModel.stratum ||
                 serverRefIdField.text !== ntpModel.serverRefId
        anchors.right: parent.right;
        text: qsTr("Apply")
        onClicked: {
            device.send(card.ntpConfigurationCommand(portModel, networkConfigurationIndex, comboBox.currentIndex, serverRefIdField.text))
            controlPopup.close()
        }
    }
}

