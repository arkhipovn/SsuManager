import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Material.impl 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls

import QCC.Controls 1.0 as QCC

import Models 1.0
import Theme 1.0

import SSU 1.0

//@N мб сделать - Item?
ColumnLayout {
    id: maskDelegate
    spacing: Theme.smallIndent

    property var model: null
    onModelChanged: {
        maskDataTable.changed = false
        maskNameTextField.changed = false
    }

    property alias nameTextFieldReadOnly: maskNameTextField.readOnly
    property bool dataReadOnly: false

    property var intervals: []
    //@ Мб вынести в настройки устройства?
    readonly property var measTypes: [ SSU.Mtie, SSU.Tdev, SSU.Rf ]

    readonly property alias maskNameChanged: maskNameTextField.changed
    readonly property alias maskDataChanged: maskDataTable.changed

    property alias incorrectMaskName: maskNameTextField.incorrectInput

    readonly property alias name: maskNameTextField.text

    function cancel() {
        maskNameTextField.changed = false
        maskDataTable.changed = false
    }

    function getName() {
        return maskNameTextField.text
    }

    function getData() {
        //@N Вот тут вообще переделать бы все.. Какая-то фигня получается. Мб сделать модель в c++ и ее передавать?
        var mtie = []
        var tdev = []
        var rf = []

        for(var i = 0; i < repeater.count; ++i) {
            var row = repeater.itemAt(i).getRow()
            if(row[0] !== null) mtie.push(row[0])
            if(row[1] !== null) tdev.push(row[1])
            if(row[2] !== null) rf.push(row[2])
        }

        var d = SSU.createEmptyData()
        d.set(SSU.Mtie, mtie)
        d.set(SSU.Tdev, tdev)
        d.set(SSU.Rf, rf)
        return d
    }

    QCC.TextField {
        id: maskNameTextField
        Layout.fillWidth: true
        placeholderText: qsTr("Mask name")
        elevation: 2

        property bool changed: false
        onTextEdited: {
            if(model != null && text === model.name) {
                changed = false
            }
            else changed = true
        }

        message: incorrectInput ? qsTr("A mask with the same name already exists")
                                : ""

        Binding {
            target: maskNameTextField
            when: !maskDataTable.changed && !maskNameTextField.changed
            property: "text"
            value: maskDelegate.model == null ? ""
                                              : maskDelegate.model.name
        }
    }   // TextField maskName


    //    Pane {    // Почему-то падает..
    //        Material.elevation: 2
    //        topPadding: 1; bottomPadding: 1
    //        leftPadding: 1; rightPadding: 1

    Rectangle {
        id: maskDataTable
        Layout.fillWidth: true; //Layout.fillHeight: true
        implicitHeight: column.height

        property bool changed: false


        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 2
        }

        Column {
            id: column
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter

            Controls.TableHeader {
                width: parent.width

                Controls.TableCell {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 1
                    color: Material.accentColor
                    text: qsTr("τ, s")
                }

                Repeater {
                    model: maskDelegate.measTypes

                    delegate: Controls.TableCell {
                        Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: 2
                        color: Material.accentColor
                        text: SSU.measDataTypeToString(modelData)

                        Controls.Separator { orientation: Qt.Vertical; }
                    }
                }
            }


            Controls.Separator { orientation: Qt.Horizontal; }

            Repeater {
                id: repeater
                model: maskDelegate.intervals.length

                delegate: Controls.RowLayoutPane {
                    width: parent.width
                    seperatorActive: true
                    spacing: 0
                    Material.elevation: 3

                    //@N Вот тут вообще переделать бы все.. Какая-то фигня получается. Мб сделать модель в c++ и ее передавать?
                    function getRow() {
                        let array = []
                        for(var i = 0; i < rowRepeater.count; ++i) {
                            var item = rowRepeater.itemAt(i)
                            var value = !item.visible ? null
                                                      : item.acceptableInput ? item.text
                                                                             : NaN
                            array.push(value)
                        }
                        return array
                    }

                    readonly property int intervalIndex: modelData

                    Controls.TableCell {
                        Layout.fillWidth: true; Layout.fillHeight: true;
                        Layout.preferredWidth: 1
                        text: maskDelegate.intervals[intervalIndex]
                    }

                    Repeater {
                        id: rowRepeater
                        model: maskDelegate.measTypes

                        delegate: Controls.TableTextField {
                            id: textField
                            Layout.fillWidth: true; Layout.fillHeight: true;
                            Layout.preferredWidth: 2

                            readOnly: maskDelegate.dataReadOnly

                            readonly property int measType: modelData

                            validator: DoubleValidator {
                                locale: "en_EN"
                                notation: DoubleValidator.ScientificNotation
                                bottom: measType == SSU.Rf ? 0
                                                           : 1e-25
                            }

                            onTextEdited: maskDataTable.changed = true

                            Binding {
                                target: textField
                                when: !maskDataTable.changed
                                property: "text"

                                value: {
                                    if(maskDelegate.model == null)
                                        return ""
                                    let value = maskDelegate.model.data.value(measType, intervalIndex)
                                    return isNaN(value) ? ""
                                                        : programSettings.displayingValuesWithoutCoefficients ? value
                                                                                                              : value.toLocaleString(Qt.locale(), 'g', 4)
                                }
                            }

                            Controls.Separator { orientation: Qt.Vertical; }
                        }
                    }
                }   // RowLayoutPane delegate
            }   // Repeater rows
        }   // Column
    }   // Item


}



