import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs
import Theme 1.0

import QtQuick.Layouts 1.12
import QCC.Controls 1.0 as QCC


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

            readonly property bool restartedParamChanged: Theme.scale != scaleSlider.value ||
                                                          languageComboBox.value != translator.currentLanguage ||
                                                          timeSpecComboBox.model[timeSpecComboBox.currentIndex] != programSettings.timeSpec ||
                                                          programSettings.timeFormat !== timeFormatComboBox.currentText ||
                                                          programSettings.dateFormat !== dateFormatComboBox.currentText

            visible: restartedParamChanged

            onClicked: {
                if(restartedParamChanged)
                    applyConfirmationPopup.open()
                else {
                    saveSettings()
                    page.comeback()
                }
            }
        }
    }

    function saveSettings() {

    }

    function saveRestartedParam() {
        if(Theme.scale != scaleSlider.value)
            Theme.scale = scaleSlider.value
        if(languageComboBox.value != translator.currentLanguage)
            translator.currentLanguage = languageComboBox.model[languageComboBox.currentIndex]
        if(timeSpecComboBox.model[timeSpecComboBox.currentIndex] != programSettings.timeSpec)
            programSettings.timeSpec = timeSpecComboBox.model[timeSpecComboBox.currentIndex]
        if(programSettings.timeFormat !== timeFormatComboBox.currentText)
            programSettings.timeFormat = timeFormatComboBox.currentText
        if(programSettings.dateFormat !== dateFormatComboBox.currentText)
            programSettings.dateFormat = dateFormatComboBox.currentText
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

        //        ScrollBar.vertical: ScrollBar { id: scrollbar; visible: flickable.interactive; }

        ColumnLayout {
            id: settingsColumn
            width: parent.width
            spacing: 0//Theme.smallIndent

            // Scale
            Label {
                text: qsTr("Scale")
                color: Material.accent
            }

            Slider {
                id: scaleSlider
                Layout.fillWidth: true

                snapMode: Slider.SnapAlways
                from: 70
                to: 200
                stepSize: 10
                value: Theme.scale// programSettings.scale

                background: Rectangle {
                    x: scaleSlider.leftPadding
                    y: scaleSlider.topPadding + scaleSlider.availableHeight / 2 - height / 2
                    width: scaleSlider.availableWidth
                    height: Theme.pixelSize * 0.7
                    radius: 2
                    color: Material.hintTextColor//Material.shade(Material.foreground, Material.Shade100)

                    Label {
                        text: scaleSlider.value
                        anchors { bottom: parent.top; horizontalCenter: parent.horizontalCenter; bottomMargin: Theme.smallIndent }
                        color: Material.shade(Material.accent, Material.Shade300)
                    }
                }
            }   // Slider scale


            // Language
            Label {
                Layout.topMargin: Theme.indent
                text: qsTr("Language")
                color: Material.accent
            }


            Controls.ComboBox {
                id: languageComboBox
                Layout.fillWidth: true
                value: translator.currentLanguage
                model: translator.languages
                delegateTextFunc: translator.languageToString
            }

            // Time
            Label {
                Layout.topMargin: Theme.indent
                text: qsTr("Time specification")
                color: Material.accent
            }

            ComboBox {
                id: timeSpecComboBox
                Layout.fillWidth: true
                currentIndex: programSettings.timeSpec == Qt.UTC ? 0 :
                                                                   programSettings.timeSpec == Qt.LocalTime ? 1
                                                                                                            : 2
                model: [ Qt.UTC, Qt.LocalTime ]
                displayText: model[currentIndex] === Qt.UTC ? qsTr("UTC")
                                                            : model[currentIndex] === Qt.LocalTime ? qsTr("Local time")
                                                                                                   : qsTr("Offset from UTC")

                delegate: ItemDelegate {
                    width: timeSpecComboBox.width; height: timeSpecComboBox.height

                    contentItem: Label {
                        text: timeSpecComboBox.model[index] === Qt.UTC ? qsTr("UTC")
                                                                       : timeSpecComboBox.model[index] === Qt.LocalTime ? qsTr("Local time")
                                                                                                                        : qsTr("Offset from UTC")
                        font: timeSpecComboBox.font
                        verticalAlignment: Text.AlignVCenter
                        color: index == timeSpecComboBox.currentIndex ? Material.accent : Material.foreground
                    }

                    highlighted: timeSpecComboBox.highlightedIndex === index
                }
            }

            Label {
                text: qsTr("Time and date format")
                color: Material.accent
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.middleIndent

                ComboBox {
                    id: dateFormatComboBox
                    Layout.fillWidth: true

                    model: [ "dd.MM.yyyy", "dd-MM-yyyy", "dd/MM/yyyy",
                        "yyyy.MM.dd", "yyyy-MM-dd",
                        "MM.dd.yyyy", "MM-dd-yyyy" ]

                    currentIndex: {
                        for(var i = 0; i < count; ++i)
                            if(model[i] === programSettings.dateFormat) return i;
                        return -1
                    }
                }

                ComboBox {
                    id: timeFormatComboBox
                    Layout.fillWidth: true

                    model: [ "HH:mm:ss", "HH:mm", "HH" ]

                    currentIndex: {
                        for(var i = 0; i < count; ++i)
                            if(model[i] === programSettings.timeFormat) return i;
                        return -1
                    }
                }
            }
        }   // Layout
    }   // Flickable
}
