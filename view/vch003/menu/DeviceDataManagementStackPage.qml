import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Dialogs 1.0 as Dialogs
import Controls 1.0 as Controls

import Theme 1.0

import QCC.Controls 1.0 as QCC
import QCC.Fonts 1.0

import Models 1.0
import SSU 1.0

Controls.StackPage {
    id: page

    property var device: null

    readonly property string backupDirPath: device == null ? SSU.deviceBackupDirPath()
                                                           : SSU.deviceBackupDirPath() + "/" + device.connectionSettings.name()

    enum LogType {
        EventLog = 0,
        MeasLog,
        UserLog
    }

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()
    }

    Controls.ToolTip {
        id: errorToolTip
        function view(str, color = Material.primary) {
            errorToolTip.Material.background = color
            text = str;
            open()
        }
        delay: 0; timeout: 4000
    }

    // Если сейчас идет загрузка данных, открывается данное окно
    Loader {
        id: loadingPopupLoader

        width: Theme.pixelSize * 30;
        x: parent.width / 2 - width / 2; y: Theme.indent

        active: device == null ? false
                               : device.fileLoading /*||
                                 device.measLogLoading ||
                                 device.eventLogLoading ||
                                 device.userActivityLogLoading*/

        onLoaded: item.open()
        sourceComponent: Popup {
            id: loadingPopup
            width: parent.width
            leftMargin: Theme.indent; rightMargin: Theme.indent
            closePolicy: Menu.NoAutoClose
            onClosed: loadingPopupLoader.active = false
            modal: true
            contentItem: Column {
                width: parent.width
                spacing: Theme.middleIndent

                Label {
                    width: parent.width
                    wrapMode: Label.WordWrap
                    horizontalAlignment: Qt.AlignHCenter
                    text: qsTr("Data loading is in progress. Wait, please")
                }

                QCC.WaitingBar {
                    duration: 2000
                    width: parent.width
                    running: true
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.pixelSize * 5
                    text: qsTr("OK")
                    onClicked: {
                        loadingPopup.close()
                        page.comeback()
                    }
                }
            }
        }
    }

    RowLayout {
        width: parent.width; height: parent.height
        spacing: Theme.indent

        GroupBox {
            clip: true
            Layout.preferredWidth: 1
            Layout.fillWidth: true; Layout.fillHeight: true
            title: qsTr("Backup")

            contentItem: GridLayout {
                rowSpacing: Theme.middleIndent

                QCC.SymbolButton {
                    Layout.row: 0; Layout.column: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth

                    symbol: "\uf0ed"
                    text: qsTr("Download")
                    topInset: 0; bottomInset: 0
                    onClicked: downloadConfirmation.open()

                    Dialogs.ConfirmationPopup {
                        id: downloadConfirmation
                        text: qsTr("The download will take some time. Are you sure?")
                        onClicked: device.downloadBackup(page.backupDirPath)
                    }
                }

                QCC.SymbolButton {
                    Layout.row: 0; Layout.column: 1
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth

                    symbol: "\uf0ee"
                    text: qsTr("Select a backup")
                    topInset: 0; bottomInset: 0
                    onClicked: {
                        var dir = backupListModel.selectBackupDir()
                        if(dir.length !== 0)
                            checkPasswordbackupLoaderPopup.openPopup(dir)
                    }
                }

                Label {
                    Layout.row: 1; Layout.column: 0
                    Layout.columnSpan: 2
                    Layout.fillWidth: true
                    wrapMode: Label.WordWrap
                    text: qsTr("Backup list")
                    color: Material.accentColor
                }

                ListView {
                    Layout.row: 2; Layout.column: 0
                    Layout.columnSpan: 2
                    clip: true
                    Layout.fillWidth: true; Layout.fillHeight: true
                    flickableDirection: Flickable.VerticalFlick
                    interactive: height < contentHeight

                    model: BackupListModel {
                        id: backupListModel
                        path: page.backupDirPath
                    }

                    delegate: ItemDelegate {
                        id: itemDelegate
                        leftPadding: Theme.indent; rightPadding: Theme.indent
                        width: ListView.view.width

                        onClicked: controlPopup.openPopup(name)

                        contentItem: RowLayout {
                            Label {
                                text: name
                                Layout.fillWidth: true
                                elide: Qt.ElideRight
                            }

                            Label {
                                Layout.leftMargin: Theme.indent
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                text: dataSize
                                color: Qt.lighter(Material.foreground, 2)
                            }

                            Label {
                                Layout.leftMargin: Theme.smallIndent
                                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                font.family: CFontLoader.awesome.name
                                text: "\uf105"
                            }
                        }   // RowLayout
                    }   // ItemDelegate

                    Dialogs.SettingsPopup {
                        id: controlPopup

                        function openPopup(backupPath) {
                            this.backupPath = backupPath
                            open()
                        }

                        property string backupPath: ""

                        leftPadding: 0; rightPadding: 0
                        title: backupPath

                        contentItem: Column {
                            spacing: 0

                            ItemDelegate {
                                width: parent.width
                                text: qsTr("Restore");
                                onClicked: {
                                    checkPasswordbackupLoaderPopup.openPopup(backupListModel.getDir(controlPopup.backupPath))
                                    controlPopup.close()
                                }
                            }

                            ItemDelegate {
                                width: parent.width
                                text: qsTr("Show in explorer");
                                onClicked: {
                                    backupListModel.openDir(controlPopup.backupPath)
                                    controlPopup.close()
                                }
                            }

                            ItemDelegate {
                                width: parent.width
                                text: qsTr("Delete")
                                Material.foreground: Material.Red

                                onClicked: delBackupMenu.open()

                                Dialogs.ConfirmationPopup {
                                    id: delBackupMenu
                                    text: qsTr("The backup will be deleted. Are you sure?")
                                    onClicked: {
                                        backupListModel.remove(controlPopup.backupPath)
                                        controlPopup.close()
                                    }
                                }
                            }
                        }
                    }   // SettingsPopup
                }   // ListView

            }   // GridLayout
        }   // GroupBox

        GroupBox {
            id: dataGroupBox
            Layout.preferredWidth: 1
            Layout.fillWidth: true; Layout.fillHeight: true
            title: qsTr("Data management")
            clip: true

            GridLayout {
                width: parent.width;
                columns: 2
                columnSpacing: 0; rowSpacing: 0

                QCC.SymbolButton {
                    Layout.row: 0; Layout.column: 0
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf0ed"
                    text: qsTr("Download")
                    toolTip.text: qsTr("Download measurement data")
                    onClicked: downloadMenu.openMenu(DeviceDataManagementStackPage.LogType.MeasLog)
                }

                QCC.SymbolButton {
                    Layout.row: 0; Layout.column: 1
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf1f8"
                    color: Material.color(Material.Red)
                    text: qsTr("Clear measurement data")
                    onClicked: clearLogTableCheckPasswordPopup.openMenu(DeviceDataManagementStackPage.LogType.MeasLog)
                }

                QCC.SymbolButton {
                    Layout.row: 1; Layout.column: 0
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf0ed"
                    text: qsTr("Download")
                    toolTip.text: qsTr("Download user activity log")
                    onClicked: downloadMenu.openMenu(DeviceDataManagementStackPage.LogType.UserLog)
                }

                QCC.SymbolButton {
                    Layout.row: 1; Layout.column: 1
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf1f8"
                    color: Material.color(Material.Red)
                    text: qsTr("Clear user activity log")
                    onClicked: clearLogTableCheckPasswordPopup.openMenu(DeviceDataManagementStackPage.LogType.UserLog)
                }

                QCC.SymbolButton {
                    Layout.row: 2; Layout.column: 0
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf0ed"
                    text: qsTr("Download")
                    toolTip.text: qsTr("Download event log")
                    onClicked: downloadMenu.openMenu(DeviceDataManagementStackPage.LogType.EventLog)
                }

                QCC.SymbolButton {
                    Layout.row: 2; Layout.column: 1
                    topInset: 0; bottomInset: 0
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    symbol: "\uf1f8"
                    color: Material.color(Material.Red)
                    text: qsTr("Clear event log")
                    onClicked: clearLogTableCheckPasswordPopup.openMenu(DeviceDataManagementStackPage.LogType.EventLog)
                }

                Controls.Separator {
                    Layout.fillWidth: true
                    Layout.row: 3; Layout.column: 0
                    Layout.columnSpan: 2
                }

                Button {
                    Layout.row: 4; Layout.column: 0
                    Layout.columnSpan: 2
                    Layout.fillWidth: true; Layout.maximumWidth: implicitWidth
                    topInset: Theme.indent
                    bottomInset: Theme.indent
                    text: qsTr("Reset settings to default values")
                    Material.background: Material.Red
                    Material.foreground: "white"
                    onClicked: resetDataCheckPasswordPopup.open()
                }   // Reset button
            }   // GridLayout

        }   // GroupBox

    }   // RowLayout

    Dialogs.CheckPasswordPopup {
        id: checkPasswordbackupLoaderPopup

        function openPopup(dir) {
            this.dir = dir
            open()
        }

        property string dir: ""

        title: qsTr("Are you sure you want to download the backup?")
        text: qsTr("All current data will be lost")
        password: device == null ? ""
                                 : device.connectionSettings.password
        applyButtonText: qsTr("Upload")

        onClicked: {
            if(!device.uploadBackup(dir))
                errorToolTip.view(qsTr("No backup data found!"), Material.color(Material.Orange, Material.Shade900))
        }
    }

    //    // всплывающее окно загрузки..
    Dialogs.DownloadDataMenu {
        id: downloadMenu
        width: Theme.pixelSize * 20
        x: dataGroupBox.x + Theme.indent; y: dataGroupBox.y + Theme.indent
        onDownloadData: downloadLogConfirmation.open()

        property int logType: DeviceDataManagementStackPage.LogType.EventLog

        function openMenu(logType) {
            this.logType = logType
            open()
        }

        Dialogs.ConfirmationPopup {
            id: downloadLogConfirmation
            text: qsTr("The download will take some time. Are you sure?")

            onClicked: {
                switch(downloadMenu.logType) {
                case DeviceDataManagementStackPage.LogType.EventLog: device.downloadEventLog(downloadMenu.time1, downloadMenu.time2); break;
                case DeviceDataManagementStackPage.LogType.MeasLog: device.downloadReclist(downloadMenu.time1, downloadMenu.time2); break;
                case DeviceDataManagementStackPage.LogType.UserLog: device.downloadUserLog(downloadMenu.time1, downloadMenu.time2); break;
                default: break;
                }
            }
        }
    }

    Dialogs.CheckPasswordPopup {
        id: resetDataCheckPasswordPopup

        applyButtonText: qsTr("Reset")

        title: qsTr("Are you sure you want to reset the device settings?")
        text: qsTr("The data will be deleted permanently! To confirm the action, enter the password again")
        password: device == null ? ""
                                 : device.connectionSettings.password
        onClicked: device.resetDevice()
    }

    Dialogs.CheckPasswordPopup {
        id: clearLogTableCheckPasswordPopup
        property int logType: DeviceDataManagementStackPage.LogType.EventLog

        function openMenu(logType) {
            this.logType = logType
            open()
        }

        password: device == null ? ""
                                 : device.connectionSettings.password

        title: {
            switch(logType) {
            case DeviceDataManagementStackPage.LogType.EventLog: return qsTr("Are you sure you want to clear the event log from the device?")
            case DeviceDataManagementStackPage.LogType.MeasLog: return qsTr("Are you sure you want to delete measurement data from the device?")
            case DeviceDataManagementStackPage.LogType.UserLog: return qsTr("Are you sure you want to clear the user activity log from the device?")
            default: return ""
            }
        }

        text: qsTr("The data will be deleted permanently! To confirm the action, enter the password again")

        applyButtonText: qsTr("Yes")

        onClicked: {
            var isok = false
            switch(logType) {
            case DeviceDataManagementStackPage.LogType.EventLog: isok = device.resetEventLog(); break;
            case DeviceDataManagementStackPage.LogType.MeasLog: isok = device.resetMeasLog(); break;
            case DeviceDataManagementStackPage.LogType.UserLog: isok = device.resetUserActivityLog(); break;
            default: break;
            }
            if(isok)
                resetLocalDataConfirmationPopup.open()
        }
    }

    //    Очистить данные из локальной базы данных?
    Dialogs.ConfirmationPopup {
        id: resetLocalDataConfirmationPopup
        x: parent.width / 2 - width / 2;
        text: qsTr("Clear data from local database?")

        onClicked: {
            var isok = false
            switch(clearLogTableCheckPasswordPopup.logType) {
            case DeviceDataManagementStackPage.LogType.EventLog: device.clearEventsLogTable(); break;
            case DeviceDataManagementStackPage.LogType.MeasLog: device.clearMeasTable(); break;
            case DeviceDataManagementStackPage.LogType.UserLog: device.clearUserActivityLogTable(); break;
            default: break;
            }
        }

    }
}
