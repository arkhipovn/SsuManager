import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.Utils 1.0

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

        QCC.DateInput {
            readOnly: true
            width: parent.width
            placeholderText: qsTr("Device time")
            dateTime: device == null ? null
                                     : Utils.qmlDateTime(device.currentDateTime, programSettings.timeSpec)
            format: programSettings.dateTimeFormat // "dd.MM.yyyy hh:mm:ss"
        }

        Label {
            width: parent.width
            wrapMode: Label.WordWrap
            visible: device == null ? false
                                    : Math.abs(device.currentDateTime.getTime() - new Date().getTime()) > programSettings.maximumTimeDifference
            text: qsTr("The device time is different from the computer time")
            color: Material.color(Material.Red)
        }

        QCC.DateInput {
            id: dateInput
            visible: device == null ? false
                                    : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
            width: parent.width
            placeholderText: qsTr("New device time")
            dateTime: programSettings.getCurrentDateTime()
            format: programSettings.dateTimeFormat // "dd.MM.yyyy hh:mm:ss"
        }

        RowLayout {
            width: parent.width;
            visible: device == null || card === null ? false
                                                     : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

            Button {
                Layout.fillWidth: true
                text: qsTr("Synchronize time with computer")
                onClicked: {
                    device.send(card.synchronizeTimeWithComputer())
                    page.comeback()
                }
            }

            Button {
                id: applyButton
                Layout.fillWidth: true
                enabled: dateInput.acceptableInput && monitoringCard !== null
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetDateTime(programSettings.dateTimeFromString(dateInput.value)))
                    page.comeback()
                }
            }
        }
    }
}

