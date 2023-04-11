import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Theme 1.0

import QCC.Fonts 1.0

import SSU 1.0
import QCC.Dialogs 1.0

Controls.StackPage {
    id: page
    leftPadding: 0; rightPadding: 0

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()
    }

    ListView {
        width: parent.width; height: parent.height
        model: SSU.eventStatusList()

        delegate: ItemDelegate {
            width: ListView.view.width; height: Theme.rowHeight
            text: SSU.eventStatusToString(modelData)

            onClicked: colorDialogLoader.open(modelData)

            contentItem: RowLayout {
                spacing: Theme.smallIndent

                Rectangle {
                    width: height * 2; height: Theme.pixelSize
                    radius: 2
                    color: {
                        programSettings.hasAlarmColorIndication
                        return programSettings.alarmColorIndication(modelData)
                    }
                }

                Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    text: SSU.eventStatusToString(modelData)
                    elide: Qt.ElideRight
                }

                Label {
                    Layout.leftMargin: Theme.smallIndent
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    font.family: CFontLoader.awesome.name
                    text: "\uf105"
                }
            }
        }
    }

    Loader {
        id: colorDialogLoader
        active: false

        property int status: -1

        function open(status) {
            this.status = status
            active = true
        }

        sourceComponent: ColorDialog {
            title: qsTr("Choose color indication for status") + " " + SSU.eventStatusToString(colorDialogLoader.status)
            onAccepted: {
                programSettings.setAlarmColorIndication(colorDialogLoader.status, color)
                colorDialogLoader.active = false
            }
            onRejected: {
                colorDialogLoader.active = false
            }
        }
    }
}
