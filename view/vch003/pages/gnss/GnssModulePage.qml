import QtQuick 2.12

import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.Fonts 1.0

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import SSU 1.0
import QCC.Utils 1.0


Item {
    id: page
    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    RowLayout {
        width: parent.width
        spacing: Theme.indent

        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.maximumWidth: Theme.pixelSize * 40
            spacing: Theme.middleIndent

            Controls.Group {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                title: qsTr("GNSS system types")

                onClicked: if(page.controlRights) controlPopup.openPopup(systemTypeSettingsComponent, -1, title)

                contentItem: Row {
                    spacing: Theme.smallIndent

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        font { pixelSize: Theme.pixelSize * 1.4; family: CFontLoader.awesome.name; }

                        readonly property bool checked: {
                            card.gnss.selectedSystems
                            return card.gnss.isSystemSelected(SSU.GPS)
                        }

                        color: checked ? Material.accent : Material.backgroundDimColor
                        text: checked ? "\uf14a" : "\uf096"
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: SSU.satelliteSystemToString(SSU.GPS)
                        rightPadding: Theme.middleIndent
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        font { pixelSize: Theme.pixelSize * 1.4; family: CFontLoader.awesome.name; }

                        readonly property bool checked: {
                            card.gnss.selectedSystems
                            return card.gnss.isSystemSelected(SSU.GLONASS)
                        }

                        color: checked ? Material.accent : Material.backgroundDimColor
                        text: checked ? "\uf14a" : "\uf096"
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: SSU.satelliteSystemToString(SSU.GLONASS)
                        rightPadding: Theme.middleIndent
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        font { pixelSize: Theme.pixelSize * 1.4; family: CFontLoader.awesome.name; }

                        readonly property bool checked: {
                            card.gnss.selectedSystems
                            return card.gnss.isSystemSelected(SSU.BeiDou)
                        }

                        color: checked ? Material.accent : Material.backgroundDimColor
                        text: checked ? "\uf14a" : "\uf096"
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: SSU.satelliteSystemToString(SSU.BeiDou)
                        rightPadding: Theme.middleIndent
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        font { pixelSize: Theme.pixelSize * 1.4; family: CFontLoader.awesome.name; }

                        readonly property bool checked: {
                            card.gnss.selectedSystems
                            return card.gnss.isSystemSelected(SSU.Galileo)
                        }

                        color: checked ? Material.accent : Material.backgroundDimColor
                        text: checked ? "\uf14a" : "\uf096"
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: SSU.satelliteSystemToString(SSU.Galileo)
                        rightPadding: Theme.middleIndent
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Theme.middleIndent

                Controls.Group {
                    Layout.fillWidth: true
                    Layout.minimumWidth: implicitLabelWidth + leftPadding + rightPadding
                    Layout.maximumWidth: implicitWidth
                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop

                    title: qsTr("Status")

                    contentItem: ColumnLayout {
                        spacing: Theme.smallIndent

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("NMEA")
                            font.bold: true
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: card == null ? ""
                                               : card.gnss.nmeasDataValidity ? qsTr("The data is correct")
                                                                             : qsTr("Data is unreliable")
                            color: card == null ? ""
                                                : card.gnss.nmeasDataValidity ? Material.color(Material.LightGreen)
                                                                              : Material.color(Material.Red)
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Tracking satellites")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.trackingSatellites
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("PDOP")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.pdop
                        }

                        Label {
                            id: dateTimeLabel
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            property var dateTime: null

                            Binding {
                                target: dateTimeLabel
                                property: "dateTime"
                                value: card == null ? null
                                                    : card.gnss.currentDateTime
                            }

                            Timer {
                                running: card != null && dateTimeLabel.dateTime != null
                                repeat: true
                                interval: 500
                                onTriggered: {
                                    dateTimeLabel.dateTime = new Date(dateTimeLabel.dateTime.getTime() + interval)
                                    dateTimeLabel.dateTimeChanged()
                                }
                            }

                            text: dateTime == null ? ""
                                                   : Qt.formatDateTime(Utils.qmlDateTime(dateTime, programSettings.timeSpec),
                                                                       programSettings.dateTimeFormat)

                            font.bold: true
                            color: Material.accentColor
                            topPadding: Theme.smallIndent
                            bottomPadding: Theme.smallIndent
                        }
                    }
                }

                Controls.Group {
                    Layout.fillWidth: true
                    Layout.minimumWidth: implicitLabelWidth + leftPadding + rightPadding
                    Layout.maximumWidth: implicitWidth
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    title: qsTr("Location")

                    contentItem: ColumnLayout {
                        spacing: Theme.smallIndent

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Latitude")
                            font.bold: true
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: card == null ? ""
                                               : SSU.hemisphereToString(card.gnss.latitudeHemisphere) + " " + card.gnss.latitude
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Longitude")
                            font.bold: true
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: card == null ? ""
                                               : SSU.hemisphereToString(card.gnss.longitudeHemisphere) + " " + card.gnss.longitude
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Altitude")
                            font.bold: true
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: card == null ? ""
                                               : card.gnss.altitude + " " + qsTr("m")
                        }
                    }
                }

                Controls.Group {
                    Layout.fillWidth: true
                    Layout.minimumWidth: implicitLabelWidth + leftPadding + rightPadding
                    Layout.maximumWidth: implicitWidth
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop

                    title: qsTr("Configuration")

                    onClicked: if(page.controlRights) controlPopup.openPopup(configurationSettingsComponent, -1, title)

                    contentItem: ColumnLayout {
                        spacing: Theme.smallIndent

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("PDOP Mask")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.pdopMask
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Elevation Mask")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.elevationMask + " °"
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("SNR Mask")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.snrMask + " " + qsTr("dB-Hz")
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Minimum number of satellites with SNR mask")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.snr
                        }

                        Label {
                            Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                            wrapMode: Label.WordWrap
                            text: qsTr("Cable delay")
                            font.bold: true
                        }

                        Label {
                            text: card == null ? ""
                                               : card.gnss.cableDelay + " " + qsTr("ns")
                        }
                    }
                }

            }   // RowLayout
        }   // Column


        Column {
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            Controls.Button {
                size: Theme.pixelSize * 0.9
                clickable: false
                symbol: card == null ? "0"
                                     : card.gnss.numberOfVisibleGpsSat
                text: SSU.satelliteSystemToString(SSU.GPS)
            }

            Controls.Button {
                size: Theme.pixelSize * 0.9
                clickable: false
                symbol: card == null ? "0"
                                     : card.gnss.numberOfVisibleGlonassSat
                text: SSU.satelliteSystemToString(SSU.GLONASS)
            }

            Controls.Button {
                size: Theme.pixelSize * 0.9
                clickable: false
                symbol: card == null ? "0"
                                     : card.gnss.numberOfVisibleBeiDouSat
                text: SSU.satelliteSystemToString(SSU.BeiDou)
            }

            Controls.Button {
                size: Theme.pixelSize * 0.9
                clickable: false
                symbol: card == null ? "0"
                                     : card.gnss.numberOfVisibleGalileoSat
                text: SSU.satelliteSystemToString(SSU.Galileo)
            }
        }

        //        Item { Layout.fillWidth: true; }   // Чтобы заполнить пространство
    }   // RowLayout



    Component {
        id: systemTypeSettingsComponent

        Column {
            spacing: 0

            CheckBox {
                id: gpsCheckBox
                topInset: 0; bottomInset: 0
                text: SSU.satelliteSystemToString(SSU.GPS)
                checked: {
                    card.gnss.selectedSystems
                    return card.gnss.isSystemSelected(SSU.GPS)
                }
            }

            CheckBox {
                id: glonassCheckBox
                topInset: 0; bottomInset: 0
                text: SSU.satelliteSystemToString(SSU.GLONASS)
                checked: {
                    card.gnss.selectedSystems
                    return card.gnss.isSystemSelected(SSU.GLONASS)
                }
            }

            CheckBox {
                id: beiDouCheckBox
                topInset: 0; bottomInset: 0
                text: SSU.satelliteSystemToString(SSU.BeiDou)
                checked: {
                    card.gnss.selectedSystems
                    return card.gnss.isSystemSelected(SSU.BeiDou)
                }
            }

            CheckBox {
                id: galileoCheckBox
                topInset: 0; bottomInset: 0
                text: SSU.satelliteSystemToString(SSU.Galileo)
                checked: {
                    card.gnss.selectedSystems
                    return card.gnss.isSystemSelected(SSU.Galileo)
                }
            }

            Label {
                width: parent.width
                wrapMode: Label.WordWrap
                text: qsTr("Only 2 satellite systems can be used at the same time")
                color: Material.accentColor
            }

            Button {
                anchors.right: parent.right;
                text: qsTr("Apply")
                enabled: gpsCheckBox.checked + glonassCheckBox.checked + beiDouCheckBox.checked + galileoCheckBox.checked < 3

                onClicked: {
                    var list = []
                    if(gpsCheckBox.checked)
                        list.push(SSU.GPS)
                    if(glonassCheckBox.checked)
                        list.push(SSU.GLONASS)
                    if(beiDouCheckBox.checked)
                        list.push(SSU.BeiDou)
                    if(galileoCheckBox.checked)
                        list.push(SSU.Galileo)
                    device.send(card.commandToSelectSystems(list))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: configurationSettingsComponent

        Column {
            QCC.TextField {
                id: pdopMaskTextField
                width: parent.width
                selectByMouse: true
                placeholderText: qsTr("PDOP Mask")
                text: card == null ? ""
                                   : card.gnss.pdopMask
                validator: DoubleValidator {
                    locale: "en_EN"
                    notation: DoubleValidator.ScientificNotation
                    bottom: 0.1
                    top: 6553.5
                }
            }

            QCC.TextField {
                id: elevationMaskTextField
                width: parent.width
                selectByMouse: true
                placeholderText: qsTr("Elevation Mask") + ", °"
                text: card == null ? ""
                                   : card.gnss.elevationMask
                validator: IntValidator {
                    bottom: 0
                    top: 90
                }
            }

            QCC.TextField {
                id: snrMaskTextField
                width: parent.width
                selectByMouse: true
                placeholderText: qsTr("SNR Mask") + ", " + qsTr("dB-Hz")
                text: card == null ? ""
                                   : card.gnss.snrMask
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
            }

            QCC.TextField {
                id: minNumOfSatSnrMaskTextField
                width: parent.width
                selectByMouse: true
                placeholderText: qsTr("Minimum number of satellites with SNR mask")
                text: card == null ? ""
                                   : card.gnss.snr
                validator: IntValidator {
                    bottom: 0
                    top: 255
                }
            }

            QCC.TextField {
                id: cableDelayTextField
                width: parent.width
                selectByMouse: true
                placeholderText: qsTr("Cable delay") + ", " + qsTr("ns")
                text: card == null ? ""
                                   : card.gnss.cableDelay
                validator: IntValidator {
                    bottom: -32768
                    top: 32767
                }
            }

            Button {
                enabled: card == null ? false
                                      : (pdopMaskTextField.acceptableInput &&
                                         elevationMaskTextField.acceptableInput &&
                                         snrMaskTextField.acceptableInput &&
                                         minNumOfSatSnrMaskTextField.acceptableInput &&
                                         cableDelayTextField.acceptableInput) &&
                                        (card.gnss.pdopMask != pdopMaskTextField.text ||
                                         card.gnss.elevationMask != elevationMaskTextField.text ||
                                         card.gnss.snrMask != snrMaskTextField.text ||
                                         card.gnss.snr != minNumOfSatSnrMaskTextField.text ||
                                         card.gnss.cableDelay != cableDelayTextField.text)

                anchors.right: parent.right;
                text: qsTr("Apply")
                onClicked: {
                    if(card.gnss.pdopMask != pdopMaskTextField.text)
                        device.send(card.commandToSetPdopMask(pdopMaskTextField.text))

                    if(card.gnss.elevationMask != elevationMaskTextField.text)
                        device.send(card.commandToSetElevationMask(elevationMaskTextField.text))

                    if(card.gnss.snrMask != snrMaskTextField.text)
                        device.send(card.commandToSetSnrMask(snrMaskTextField.text))

                    if(card.gnss.snr != minNumOfSatSnrMaskTextField.text)
                        device.send(card.commandToSetSnr(minNumOfSatSnrMaskTextField.text))

                    if(card.gnss.cableDelay != cableDelayTextField.text)
                        device.send(card.commandToSetAntennaCableDelay(cableDelayTextField.text))

                    controlPopup.close()
                }
            }
        }
    }

}


