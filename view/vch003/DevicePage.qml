import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import Menu 1.0
import Theme 1.0

import Dialogs 1.0 as Dialogs
import Device 1.0 as D
import Controls 1.0 as Controls

import QCC 1.0
import QCC.Controls 1.0 as QCC
import QCC.Controls 2.0 as QCC2

import ConnectionSettings 1.0
import SSU 1.0

import "menu/"
import "menu/events"
import "menu/measurement"


D.DevicePage {
    id: devicePage

    Component.onDestruction: creator.release()

    ObjectCreator { id: creator }


    // Нужно для создания окон
    enum MenuWindows {
        InventoryMenu,
        ProgramSettingsMenu,
        UserListMenu,
        PanelConfigurationMenu,
        UserActivityLogMenu,
        EventLogMenu,
        MaskMenu,
        MeasurementMenu
    }

    // загрузчик основной страницы..
    Loader {
        id: bodyLoader
        width: parent.width; height: parent.height

        sourceComponent: {
            if(device === null || device.panel === null) // Какая-то ошибка явно..
                return null
            return device.panel.inventory.data.length > 0 ? mainFormComponent
                                                          : device.connectionSettings.status === ConnectionSettings.Connected ? dataLoadingComponent
                                                                                                                              : deviceConnectionComponent
        }
    }


    Component {
        id: deviceConnectionComponent

        Item {
            Column {
                anchors { right: parent.right; top: parent.top; margins: Theme.smallIndent; }

                Loader {
                    active: ENABLE_APP_UPDATER
                    sourceComponent: ManagerMenuItem {
                        symbolScale: 1.3
                        toolTip.text: updater.updateAvailable || updater.readyToInstall ? qsTr("Updates available") :
                                                                                          qsTr("Updates")
                        symbol: updater.updateAvailable || updater.readyToInstall ? "\uf021"
                                                                                  : "\uf05d"
                        color: updater.updateAvailable || updater.readyToInstall ? Material.color(Material.Orange)
                                                                                 : Material.color(Material.Green)
                        onClicked: controller.openUpdateWidget()
                    }
                }

                ManagerMenuItem { //@N выделить эту кнопку потом в другое место..
                    toolTip.text: qsTr("Program settings")
                    symbolScale: 1.3
                    symbol: "\uf013"
                    color: Material.color(Material.BlueGrey)
                    highlighted: creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.ProgramSettingsMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.ProgramSettingsMenu, programSettingsWindowComponent, null).requestActivate()    // devicePage
                }
            }


            DeviceConnectionPane {
                width: Theme.pixelSize * 24; height: Theme.pixelSize * 23
                anchors.centerIn: parent
                device: devicePage.device
                focus: true
                Material.elevation: 3
            }
        }
    }

    Component {
        id: dataLoadingComponent

        Item {
            Column {
                width: Theme.pixelSize * 24
                anchors.centerIn: parent
                spacing: Theme.pixelSize

                Label {
                    id: label
                    anchors.horizontalCenter: parent.horizontalCenter
                    Material.foreground: Material.accent
                    text: qsTr("Loading")
                }

                QCC2.WaitingBar {
                    id: bar
                    width: parent.width
                    running: true
                    duration: 1500
                    progressColor: Material.accentColor
                }

                Button {
                    id: disconnectButton
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: false
                    text: qsTr("Disconnect")
                    onClicked: controller.disconnectFromDevice(devicePage.device)

                    // чтобы кнопка появилась через некоторое время
                    Timer {
                        id: timer
                        interval: 3000
                        repeat: false
                        running: true
                        onTriggered: disconnectButton.visible = true
                    }
                }
            }
        }
    }

    Component {
        id: mainFormComponent

        Item {
            id: mainItem

            Component.onDestruction: {
                creator.release()
                showSignalPanel(false)
            }

            property bool resizedWindow: false

            function closeWindows() {
                mainPanel.closeAllPages()
                creator.release()
            }

            ViewSettings {
                id: viewSettings
            }

            Connections {
                target: device.connectionSettings
                onDeviceConnected:  mainItem.closeWindows() // нужно закрыть все окна и очистить данные..
                onDeviceDisconnected: mainItem.closeWindows() // нужно закрыть все окна и очистить данные..
            }

            function showSignalPanel(show) {
                if(show === signalPanelLoader.active)
                    return
                if(signalPanelLoader.active) {
                    // Открыто окно, нужно закрыть
                    if(resizedWindow) {
                        appWindow.y += signalPanelLoader.height
                        appWindow.height -= signalPanelLoader.height
                    }
                    signalPanelLoader.active = false
                }
                else {
                    // Свернута верхняя панель, нужно открыть
                    var signalPanelHeight = mainPanel.height / 2
                    var newHeight = appWindow.height + signalPanelHeight

                    if(newHeight * 1.1 > Screen.height) {
                        resizedWindow = false
                    }
                    else {
                        if(appWindow.y > signalPanelHeight * 1.1) {
                            appWindow.y = Math.max(appWindow.y - signalPanelHeight, 50)
                        }
                        else if(appWindow.y + newHeight > Screen.height) {  // вышли за пределы экрана. Нужно сдвинуть y
                            appWindow.y = Math.max(Screen.height - newHeight, 50)
                        }

                        resizedWindow = true
                        appWindow.height = newHeight
                    }
                    signalPanelLoader.active = true
                }
            }

            ColumnLayout {
                anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: menu.left; margins: Theme.smallIndent; }
                spacing: Theme.smallIndent

                Label {
                    id: timeErrorLabel

                    visible: device == null ? false
                                            : Math.abs(device.currentDateTime.getTime() - new Date().getTime()) > programSettings.maximumTimeDifference

                    Layout.fillWidth: true;
                    wrapMode: Label.WordWrap
                    text: qsTr("Attention! The device time is different from the computer time")
                    color: Material.color(Material.Red)
                    padding: Theme.smallIndent
                    horizontalAlignment: Label.AlignHCenter
                }

                // Панель с сигналами
                Loader {
                    id: signalPanelLoader
                    active: false
                    visible: active
                    Layout.fillWidth: true; Layout.fillHeight: true
                    Layout.preferredHeight: 1
                    sourceComponent: SignalPanel {
                        device: devicePage.device
                    }
                }

                // главная панель
                Panel {
                    id: mainPanel
                    device: devicePage.device
                    Layout.preferredHeight: 2
                    Layout.fillWidth: true; Layout.fillHeight: true
                }

                // светофор событий
                Flow {
                    id: lightEvents
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Repeater {
                        model: Vch003.eventStatusList()

                        Controls.Button {
                            topInset: 0; bottomInset: 0
                            text: SSU.eventStatusToString(modelData)
                            color: Material.foreground
                            border.color: "#B5B5B5"
                            backgroundColor: {
                                programSettings.hasAlarmColorIndication
                                return programSettings.alarmColorIndication(modelData)
                            }

                            symbol: {
                                if(device == null)
                                    return 0
                                switch(modelData) {
                                case SSU.Critical:  return device.activeEvents.numberOfCriticalEvents
                                case SSU.Major:     return device.activeEvents.numberOfMajorEvents
                                case SSU.Minor:     return device.activeEvents.numberOfMinorEvents
                                case SSU.Warning:   return device.activeEvents.numberOfWarningEvents
                                default: return "0"
                                }
                            }

                            onClicked: {
                                let obj = creator.createComponent(DevicePage.MenuWindows.EventLogMenu, eventLogMenuWindowComponent, null)  // devicePage
                                if(obj !== null) {
                                    obj.showEvents(modelData)   // Добавим фильтр
                                    obj.requestActivate()
                                }
                            }
                        }
                    }
                }   // Flow светофор..
            }

            // Menu
            ManagerMenu {
                id: menu
                anchors { top: parent.top; bottom: parent.bottom; right: parent.right; margins: Theme.smallIndent; }

                ManagerMenuItem {
                    toolTip.text: !signalPanelLoader.active ? qsTr("Show signal panel")
                                                            : qsTr("Hide signal panel")
                    symbol: !signalPanelLoader.active ? "\uf102"   // up
                                                      : "\uf103"   // down
                    onClicked: mainItem.showSignalPanel(!signalPanelLoader.active)
                    color: Material.color(Material.Blue)
                    symbolScale: 1.4
                }

                MenuSeparator { width: parent.width; }

                ManagerMenuItem {
                    toolTip.text: qsTr("Connection settings")
                    symbol: device.connectionSettings.status === ConnectionSettings.Connected ? "\uf08b"
                                                                                              : "\uf090"
                    color: Material.color(Material.LightGreen)
                    symbolScale: 1.3
                    highlighted: connectionPopupLoader.active
                    onClicked: connectionPopupLoader.active = true
                }

                MenuSeparator { width: parent.width; }

                ManagerMenuItem {
                    toolTip.text: qsTr("Inventory")
                    symbol: "\uf1b3"
                    symbolScale: 1
                    color: Material.color(Material.BlueGrey)
                    highlighted: creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.InventoryMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.InventoryMenu, inventoryWindowComponent, null).requestActivate()    // devicePage
                }

                ManagerMenuItem {
                    toolTip.text: qsTr("Panel configuration")
                    symbol: "\uf0d0"
                    symbolScale: 1
                    color: Material.color(Material.BlueGrey)
                    highlighted: creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.PanelConfigurationMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.PanelConfigurationMenu, panelConfigurationWindowComponent, null).requestActivate()  // devicePage
                }

                MenuSeparator { width: parent.width; }

                ManagerMenuItem {
                    toolTip.text: qsTr("Events")
                    symbolScale: 1.2
                    symbol: "\uf1da"
                    color: Material.color(Material.BlueGrey)

                    //            infoChanges: controller.deviceLogModel.activeEventList.updatesAvailable || controller.deviceLogModel.eventLog.updatesAvailable

                    highlighted: creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.EventLogMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.EventLogMenu, eventLogMenuWindowComponent, null).requestActivate()  // devicePage
                }

                ManagerMenuItem {
                    toolTip.text: qsTr("Measurements")
                    symbol: "\uf1fe"
                    symbolScale: 1
                    color: Material.color(Material.BlueGrey)
                    highlighted: visible && creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.MeasurementMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.MeasurementMenu, measurementMenuWindowComponent, null).requestActivate()    // devicePage
                }

                ManagerMenuItem {
                    toolTip.text: qsTr("User activity log")

                    visible: device == null ? false
                                            : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..
                    symbolScale: 1.3
                    symbol: "\uf1ae"
                    color: Material.color(Material.BlueGrey)
                    highlighted: visible && creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.UserActivityLogMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.UserActivityLogMenu, userActivityLogWindowComponent, null).requestActivate()    // devicePage
                }

                MenuSeparator { width: parent.width; }

                ManagerMenuItem {
                    toolTip.text: qsTr("Thresholds & masks")
                    symbol: "\uf05e"
                    symbolScale: 1.3

                    color: Material.color(Material.BlueGrey)

                    highlighted: visible && creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.MaskMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.MaskMenu, maskWindowComponent, null).requestActivate()  // devicePage
                }

                ManagerMenuItem {
                    visible: device == null ? false
                                            : device.options.userRights.deviceControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

                    toolTip.text: qsTr("Users")
                    symbol: "\uf2be"
                    symbolScale: 1.1
                    color: Material.color(Material.BlueGrey)
                    highlighted: visible && creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.UserListMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.UserListMenu, userListWindowComponent, null).requestActivate()  // devicePage
                }

                ManagerMenuItem {
                    toolTip.text: qsTr("Device settings")
                    symbol: "\uf1de"
                    symbolScale: 1.3
                    color: Material.color(Material.BlueGrey)
                    visible: mainPanel.monitoringCard !== null
                    highlighted: visible && mainPanel.listOfAddress.indexOf(mainPanel.monitoringCard.address) !== -1
                    onClicked: {
                        if(mainPanel.monitoringCard === null)
                            return
                        mainPanel.openCardPage(mainPanel.monitoringCard.address)
                    }
                }

                MenuSeparator { width: parent.width; }

                ManagerMenuItem {
                    toolTip.text: qsTr("Program settings")
                    symbolScale: 1.3
                    symbol: "\uf013"
                    color: Material.color(Material.BlueGrey)
                    highlighted: creator.objectList.identifiers.indexOf(DevicePage.MenuWindows.ProgramSettingsMenu) !== -1
                    onClicked: creator.createComponent(DevicePage.MenuWindows.ProgramSettingsMenu, programSettingsWindowComponent, null).requestActivate()    // devicePage
                }

                Loader {
                    active: ENABLE_APP_UPDATER
                    sourceComponent: ManagerMenuItem {
                        symbolScale: 1.3
                        toolTip.text: updater.updateAvailable || updater.readyToInstall ? qsTr("Updates available") :
                                                                                          qsTr("Updates")
                        symbol: updater.updateAvailable || updater.readyToInstall ? "\uf021"
                                                                                  : "\uf05d"
                        color: updater.updateAvailable || updater.readyToInstall ? Material.color(Material.Orange)
                                                                                 : Material.color(Material.Green)
                        onClicked: controller.openUpdateWidget()
                    }
                }
            }   // Menu

            // всплывающее окно управления подключениями
            Loader {
                id: connectionPopupLoader
                active: false
                width: Theme.pixelSize * 24; height: Theme.pixelSize * 23
                x: parent.width * 0.5 - width * 0.5; y: parent.height * 0.5 - height * 0.5
                onLoaded: item.open()
                sourceComponent: Popup {
                    implicitWidth: parent.width; implicitHeight: parent.height
                    padding: 0; modal: true; clip: true

                    DeviceConnectionPane {
                        width: parent.width; height: parent.height
                        device: devicePage.device
                    }

                    onClosed: connectionPopupLoader.active = false
                }   // Popup
            }   // Loader

        }
    }


    // Windows

    Component {
        id: inventoryWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Inventory")
            width: Math.min(60 * Theme.pixelSize, 1000); height: Math.min(60 * Theme.pixelSize, 1000)
            settingsCategory: "InventoryWindow"

            InventoryPage {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }


    Component {
        id: maskWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Thresholds & masks")
            width: Math.min(60 * Theme.pixelSize, 1000); height: Math.min(35 * Theme.pixelSize, 1000)
            settingsCategory: "MaskWindow"

            ThresholdsAndMasksForm {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }


    Component {
        id: userListWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Users")
            width: Math.min(35 * Theme.pixelSize, 1000); height: Math.min(35 * Theme.pixelSize, 1000)
            settingsCategory: "UserListWindow"

            UserListPage {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }

    Component {
        id: programSettingsWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Program settings")
            width: Math.min(35 * Theme.pixelSize, 1000); height: Math.min(40 * Theme.pixelSize, 1000)
            settingsCategory: "ProgramSettingsWindow"

            ProgramSettingsPage {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }

    Component {
        id: userActivityLogWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("User activity log")
            settingsCategory: "UserActivityLogWindow"
            width: Math.min(90 * Theme.pixelSize, 1000); height: Math.min(50 * Theme.pixelSize, 1000)

            UserActivityLogPage {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }

    Component {
        id: measurementMenuWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Measurements")
            width: Math.min(90 * Theme.pixelSize, 1000); height: Math.min(50 * Theme.pixelSize, 1000)
            settingsCategory: "MeasurementMenuWindow"

            DeviceMeasurementPage {
                id: deviceEventsPage
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }

    Component {
        id: eventLogMenuWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Events")
            width: Math.min(90 * Theme.pixelSize, 1000); height: Math.min(50 * Theme.pixelSize, 1000)
            settingsCategory: "EventLogWindow"

            function showEvents(status) {
                deviceEventsPage.showEvents(status)
            }

            DeviceEventsPage {
                id: deviceEventsPage
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }

    Component {
        id: panelConfigurationWindowComponent

        Dialogs.ApplicationWindow {
            title: qsTr("Panel configuration")
            width: Math.min(75 * Theme.pixelSize, 1000); height: Math.min(45 * Theme.pixelSize, 1000)
            settingsCategory: "PanelConfigurationWindow"

            ViewSettings {
                id: viewSettings
            }

            PanelConfigurationPage {
                width: parent.width; height: parent.height
                device: devicePage.device
            }
        }
    }


}






