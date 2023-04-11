import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs
import Theme 1.0

import QtQuick.Layouts 1.12
import QCC.Controls 1.0 as QCC

import ConnectionSettings 1.0

Controls.StackPage {
    id: page

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()

        QCC.SymbolButton {
            color: Material.accent
            text: qsTr("Apply")
            symbol: "\uf05d"

            readonly property bool restartedParamChanged: (checkAcceptableInput(pollingIntervalTextField) &&
                                                           checkAcceptableInput(measDataPollIntervalTextField) &&
                                                           checkAcceptableInput(measLogPollIntervalTextField) &&
                                                           checkAcceptableInput(userLogPollIntervalTextField)) &&
                                                          (checkChanges(pollingIntervalTextField, programSettings.pollingInterval) ||
                                                           checkChanges(measDataPollIntervalTextField, programSettings.measDataPollInterval) ||
                                                           checkChanges(measLogPollIntervalTextField, programSettings.measLogPollInterval) ||
                                                           checkChanges(userLogPollIntervalTextField, programSettings.userLogPollInterval))

            visible: maximumTimeDifferenceTextField.acceptableInput && (
                         restartedParamChanged ||
                         maximumTimeDifferenceTextField.text != (programSettings.maximumTimeDifference / 1000) ||
                         updatingCardParametersOnEventCheckBox.checked !== programSettings.updatingCardParametersOnEventEnabled ||
                         updatingLogsOnEventCheckBox.checked !== programSettings.updatingLogsOnEventEnabled)

            onClicked: {
                if(controller.device !== null &&
                        controller.device.connectionSettings.status !== ConnectionSettings.Unconnected &&
                        restartedParamChanged) {
                    applyConfirmationPopup.open()
                }
                else {
                    saveSettings()
                    saveRestartedParam()
                    page.comeback()
                }
            }
        }
    }

    function checkAcceptableInput(textField) {
        return textField.checked ? textField.acceptableInput
                                 : true
    }

    function checkChanges(textField, value) {
        return textField.checked ? textField.text != (value / 1000)
                                 : value > 0
    }

    function saveSettings() {
        if(maximumTimeDifferenceTextField.text != (programSettings.maximumTimeDifference / 1000))
            programSettings.maximumTimeDifference = maximumTimeDifferenceTextField.text * 1000
        programSettings.setUpdatingOnEventEnabled(updatingCardParametersOnEventCheckBox.checked,
                                                  updatingLogsOnEventCheckBox.checked)
    }

    function saveRestartedParam() {
        if(checkChanges(pollingIntervalTextField, programSettings.pollingInterval)) {
            programSettings.pollingInterval = pollingIntervalTextField.checked ? pollingIntervalTextField.text * 1000
                                                                               : -programSettings.pollingInterval
        }
        if(checkChanges(measDataPollIntervalTextField, programSettings.measDataPollInterval)) {
            programSettings.measDataPollInterval = measDataPollIntervalTextField.checked ? measDataPollIntervalTextField.text * 1000
                                                                                         : -programSettings.measDataPollInterval
        }
        if(checkChanges(measLogPollIntervalTextField, programSettings.measLogPollInterval)) {
            programSettings.measLogPollInterval = measLogPollIntervalTextField.checked ? measLogPollIntervalTextField.text * 1000
                                                                                       : -programSettings.measLogPollInterval
        }
        if(checkChanges(userLogPollIntervalTextField, programSettings.userLogPollInterval)) {
            programSettings.userLogPollInterval = userLogPollIntervalTextField.checked ? userLogPollIntervalTextField.text * 1000
                                                                                       : -programSettings.userLogPollInterval
        }
    }

    Dialogs.ConfirmationPopup {
        id: applyConfirmationPopup
        text: qsTr("You need to restart for applying some of new settings")
        onClicked: {
            saveSettings()
            saveRestartedParam()
            controller.restartApplication()  // перезапуск приложения
        }
    }


    Flickable {
        id: flickable
        width: parent.width; height: parent.height

        contentHeight: settingsColumn.height
        boundsBehavior: Flickable.StopAtBounds
        interactive: contentHeight > height
        flickableDirection: Flickable.VerticalFlick

        ColumnLayout {
            id: settingsColumn
            width: parent.width
            spacing: 0

            // QCC.TextField {
            QCC.CheckableTextField {
                id: pollingIntervalTextField
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Polling interval, s")
                validator: IntValidator { bottom: 1; }

                checked: programSettings.pollingInterval > 0
                text: pollingIntervalTextField.checked ? Math.abs(programSettings.pollingInterval) / 1000
                                                       : ""
                toolTipText: checked ? qsTr("Turn off auto-update")
                                     : qsTr("Enable auto-update")
            }

            // QCC.TextField {
            QCC.CheckableTextField {
                id: measDataPollIntervalTextField
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Meas data poll interval, s")
                validator: IntValidator { bottom: 1; }

                checked: programSettings.measDataPollInterval > 0
                text: measDataPollIntervalTextField.checked ? Math.abs(programSettings.measDataPollInterval) / 1000
                                                            : ""
                toolTipText: checked ? qsTr("Turn off auto-update")
                                     : qsTr("Enable auto-update")
            }

            // QCC.TextField {
            QCC.CheckableTextField {
                id: measLogPollIntervalTextField
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Meas log poll interval, s")
                validator: IntValidator { bottom: 10; }

                checked: programSettings.measLogPollInterval > 0
                text: measLogPollIntervalTextField.checked ? Math.abs(programSettings.measLogPollInterval) / 1000
                                                           : ""
                toolTipText: checked ? qsTr("Turn off auto-update")
                                     : qsTr("Enable auto-update")
            }

            // QCC.TextField {
            QCC.CheckableTextField {
                id: userLogPollIntervalTextField
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("User log poll interval, s")
                validator: IntValidator { bottom: 10; }

                checked: programSettings.userLogPollInterval > 0
                text: userLogPollIntervalTextField.checked ? Math.abs(programSettings.userLogPollInterval) / 1000
                                                           : ""
                toolTipText: checked ? qsTr("Turn off auto-update")
                                     : qsTr("Enable auto-update")
            }

            CheckBox {
                id: updatingCardParametersOnEventCheckBox
                Layout.fillWidth: true
                topInset: 0; bottomInset: 0
                text: qsTr("Updating card parameters on event")
                Binding {
                    target: updatingCardParametersOnEventCheckBox
                    property: "checked"
                    value: programSettings.updatingCardParametersOnEventEnabled
                }
            }

            CheckBox {
                id: updatingLogsOnEventCheckBox
                Layout.fillWidth: true
                topInset: 0; bottomInset: 0
                text: qsTr("Updating logs on event")
                Binding {
                    target: updatingLogsOnEventCheckBox
                    property: "checked"
                    value: programSettings.updatingLogsOnEventEnabled
                }
            }

            QCC.TextField {
                id: maximumTimeDifferenceTextField
                Layout.fillWidth: true
                inputMethodHints: Qt.ImhDigitsOnly
                placeholderText: qsTr("Maximum time difference, s")
                text: programSettings.maximumTimeDifference / 1000
                validator: IntValidator { bottom: 1; }
            }
        }   // Layout
    }   // Flickable
}
