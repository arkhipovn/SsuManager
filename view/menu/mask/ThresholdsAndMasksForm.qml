import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Menu 1.0 as M
import Dialogs 1.0 as Dialogs
import Controls 1.0 as Controls

import Theme 1.0
import SSU 1.0

import Models.Mask 1.0

import QCC.Controls 1.0 as QCC
import QCC.Fonts 1.0

M.MenuPage {
    id: page
    rightPadding: Theme.middleIndent

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    readonly property var monitoringCard: device == null ? null
                                                         : device.panel.inventory.monitoringCard
    readonly property var outputCard: device == null ? null
                                                     : device.panel.inventory.hasData ? device.panel.inventory.findCard(SSU.Output)
                                                                                      : null

    RowLayout {
        width: parent.width; height: parent.height
        spacing: Theme.indent

        ColumnLayout {
            Layout.fillHeight: true

            Layout.fillWidth: true
            Layout.minimumHeight: Theme.pixelSize * 20
            Layout.maximumWidth: Theme.pixelSize * 20

            ListView {
                id: maskListView
                Layout.fillWidth: true; Layout.fillHeight: true
                clip: true

                property var currentMask: null

                Binding {
                    target: maskListView
                    when: maskListView.count == 0
                    property: "currentMask"
                    value: null
                }


                model: !tabBar.thresholdsTab ? controller.measMaskList
                                             : device == null ? null
                                                              : device.thresholdList

                interactive: height < contentHeight
                currentIndex: 0

                delegate: ItemDelegate {
                    leftPadding: Theme.indent; rightPadding: Theme.indent
                    width: ListView.view.width

                    onClicked: ListView.view.currentIndex = index
                    highlighted: ListView.isCurrentItem

                    Binding {
                        target: maskListView
                        when: maskListView.currentIndex == index
                        property: "currentMask"
                        value: mask
                    }

                    contentItem: Item {
                        Label {
                            anchors { left: parent.left; right: icon.left; verticalCenter: parent.verticalCenter; }
                            text: name
                            width: parent.width
                            elide: Qt.ElideRight
                        }

                        Label {
                            id: icon
                            anchors { right: parent.right; verticalCenter: parent.verticalCenter; }
                            font.family: CFontLoader.awesome.name
                            text: "\uf105"
                        }
                    }
                }
            }

            TabBar {
                id: tabBar
                Layout.leftMargin: Theme.indent
                currentIndex: page.device === null ? 1
                                                   : 0
                property bool thresholdsTab: tabBar.currentIndex == 0

                TabButton {
                    visible: page.device !== null
                    width: implicitWidth
                    text: qsTr("Thresholds")
                }

                TabButton {
                    width: implicitWidth
                    text: qsTr("Masks")
                }
            }
        }   // ColumnLayout


        // Данные маски
        Loader {
            id: maskViewLoader
            Layout.topMargin: Theme.indent
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true; //Layout.fillHeight: true

            sourceComponent: maskListView.count == 0 ? errorLabelComponent
                                                     : tabBar.thresholdsTab ? thresholdViewComponent
                                                                            : userMaskViewComponent

            property bool thresholdChangeEnabled: monitoringCard !== null &&
                                                  outputCard !== null &&
                                                  page.controlRights

            Component {
                id: errorLabelComponent

                Label {
                    padding: Theme.indent
                    horizontalAlignment: Qt.AlignHCenter
                    text: qsTr("The list is empty")
                }
            }

            Component {
                id: thresholdViewComponent

                MaskItem {
                    model: maskListView.currentMask
                    intervals: page.device == null ? []
                                                   : page.device.options.kThresholdIntervals

                    nameTextFieldReadOnly: true
                    dataReadOnly: !thresholdChangeEnabled
                    applyToAllButtonVisible: thresholdChangeEnabled

                    onAppliedToAll: {
                        device.copyThresholdsToAllChannels(maskListView.currentIndex)
                    }

                    onDataApplied: {
                        device.changeThreshold(maskListView.currentIndex, data)
                    }
                }
            }

            Component {
                id: userMaskViewComponent

                MaskItem {
                    model: maskListView.currentMask
                    intervals: page.device == null ? []
                                                   : page.device.options.kMeasIntervals

                    incorrectMaskName: controller.measMaskList.contains(name) && maskNameChanged
                    dataReadOnly: false
                    nameTextFieldReadOnly: false
                    applyToAllButtonVisible: thresholdChangeEnabled

                    onAppliedToAll: {
                        device.applyThresholdsToAllChannels(getData())
                    }

                    onApplied: {
                        controller.measMaskList.changeMask(maskListView.currentIndex, name, data)
                    }

                    onDataApplied: {
                        controller.measMaskList.setMaskData(maskListView.currentIndex, data)
                    }

                    onNameApplied: {
                        controller.measMaskList.reneme(maskListView.currentIndex, name)
                    }
                }
            }
        }

        // Дополнительные настройки
        Loader {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: Theme.indent
            Layout.preferredWidth: Theme.pixelSize * 20
            sourceComponent: tabBar.thresholdsTab ? thresholdsSettings
                                                  : userMaskSettings
        }

        Component {
            id: thresholdsSettings

            Column {
                spacing: 0

                QCC.LinkButton {
                    text: qsTr("Update")
                    onClicked: device.updateThresholds()
                }

                QCC.LinkButton {
                    text: qsTr("Save to file")
                    onClicked: addMaskPopupLoader.open(maskListView.currentMask)
                }

                Loader {
                    visible: active
                    active: maskViewLoader.thresholdChangeEnabled
                    sourceComponent: QCC.LinkButton {
                        text: qsTr("Reset")
                        color: Material.color(Material.Red)
                        onClicked: resetThresholdsCheckPasswordPopup.open()
                        Dialogs.CheckPasswordPopup {
                            id: resetThresholdsCheckPasswordPopup

                            applyButtonText: qsTr("Reset")
                            title: qsTr("Reset thresholds?")
                            password: device == null ? ""
                                                     : device.connectionSettings.password
                            onClicked: device.resetThresholds()
                        }
                    }
                }

                Loader {
                    width: parent.width
                    visible: active
                    active: maskViewLoader.thresholdChangeEnabled
                    sourceComponent: CheckBox {
                        id: measurementModeCheckBox

                        text: qsTr("Enable thresholds")
                        topInset: 0; bottomInset: 0

                        Binding {
                            target: measurementModeCheckBox
                            property: "checked"
                            value: monitoringCard === null ? false
                                                           : monitoringCard.thresholdsEnabled
                        }

                        onClicked: device.send(monitoringCard.enableThresholdCommand(checked))
                    }
                }

                Loader {
                    width: parent.width
                    visible: active
                    active: monitoringCard === null ? false
                                                    : outputCard === null || !monitoringCard.thresholdsEnabled
                    sourceComponent: Label {
                        topPadding: Theme.indent
                        wrapMode: Label.WordWrap
                        text: outputCard === null ? qsTr("Editing is not possible. No") + " " + SSU.cardTypeToString(SSU.Output)
                                                  : qsTr("Attention. Thresholds disabled")
                    }
                }
            }
        }

        Component {
            id: userMaskSettings

            Column {
                spacing: 0

                QCC.LinkButton {
                    text: qsTr("Update")
                    onClicked: controller.measMaskList.update()
                }

                QCC.LinkButton {
                    text: qsTr("Add mask")
                    onClicked: addMaskPopupLoader.open(null)
                }

                QCC.LinkButton {
                    text: qsTr("Remove current mask")
                    color: Material.color(Material.Red)
                    onClicked: controller.measMaskList.removeMask(maskListView.currentIndex)
                }

                //                QCC.SymbolButton {
                //                    text: qsTr("Update")
                //                    textColor: Material.accentColor
                //                    onClicked: controller.measMaskList.update()
                //                    topInset: 0; bottomInset: 0
                //                }

                //                QCC.SymbolButton {
                //                    text: qsTr("Add mask")
                //                    textColor: Material.accentColor
                //                    onClicked: addMaskPopupLoader.open(null)
                //                    topInset: 0; bottomInset: 0
                //                }

                //                QCC.SymbolButton {
                //                    text: qsTr("Remove current mask")
                //                    color: Material.color(Material.Red)
                //                    symbol: "\uf014"
                //                    onClicked: controller.measMaskList.removeMask(maskListView.currentIndex)
                //                    topInset: 0; bottomInset: 0
                //                }
            }
        }
    }   // RowLayout contentItem


    Loader {
        id: addMaskPopupLoader
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(parent.width, Theme.pixelSize * 50)
        active: false
        onLoaded: item.open()

        function open(mask) {
            this.mask = mask
            active = true
        }

        onActiveChanged: {
            if(!active)
                mask = null
        }

        property var mask: null

        sourceComponent: Dialogs.SettingsPopup {
            id: settingsPopup
            width: addMaskPopupLoader.width
            title: qsTr("Adding a mask")
            onClosed: addMaskPopupLoader.active = false

            MaskView {
                id: maskDelegate
                width: parent.width; height: parent.height

                intervals: page.device == null ? []
                                               : page.device.options.kMeasIntervals
                model: addMaskPopupLoader.mask

                nameTextFieldReadOnly: false
                dataReadOnly: false
                incorrectMaskName: controller.measMaskList.contains(name) && maskNameChanged

                Row {
                    Layout.alignment: Qt.AlignRight

                    QCC.SymbolButton {
                        id: applyButton
                        text: qsTr("Apply")
                        enabled: maskDelegate.name.length > 0 && !maskDelegate.incorrectMaskName
                        onClicked: {
                            controller.measMaskList.createMask(maskDelegate.getName(), maskDelegate.getData())
                            settingsPopup.close()
                        }
                    }

                    Loader {
                        active: maskDelegate.maskNameChanged || maskDelegate.maskDataChanged
                        visible: active
                        sourceComponent: QCC.SymbolButton {
                            text: qsTr("Cancel")
                            onClicked: maskDelegate.cancel()
                        }
                    }
                }
            }
        }
    }

}
