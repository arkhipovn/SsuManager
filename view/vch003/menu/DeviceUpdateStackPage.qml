import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Dialogs 1.0 as Dialogs
import Controls 1.0 as Controls
import Theme 1.0

import QCC.Controls 1.0 as QCC

import SSU 1.0

Controls.StackPage {
    id: page

    property var device: null

    header: Controls.StackPageHeader {
        id: stackHeader
        title: page.title
        onComeback: page.comeback()
    }

    // Если сейчас идет загрузка данных, открывается данное окно
    Loader {
        id: loadingPopupLoader

        width: Theme.pixelSize * 30;
        x: parent.width / 2 - width / 2; y: Theme.indent

        active: device == null ? false
                               : device.fileLoading

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

    ListView {
        width: parent.width; height: parent.height
        model: device == null ? null
                              : device.panel.inventory.data
        delegate: cardDelegateComponent
        spacing: Theme.smallIndent

        header: Label {
            width: parent.width;
            bottomPadding: Theme.indent
            wrapMode: Label.WordWrap
            text: qsTr("Click on the upload button or transfer the file to the desired card")
            background: Rectangle {}
        }
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


    Component {
        id: cardDelegateComponent

        Loader {
            width: ListView.view.width; //height: active && status === Loader.Ready ? Theme.rowHeight * 1.8 : 0
            active: type !== SSU.Stub && type !== SSU.InvalidType

            sourceComponent: DropArea {
                id: dropArea
                height: Theme.rowHeight * 1.8

                enabled: device == null ? false
                                        : !device.fileLoading

                onEntered: {
                    if(drag.urls.length === 1) {
                        var file = drag.urls[0].replace("file:///", '')
                        if(file.includes(".hex"))
                            return
                    }
                    drag.accepted = false;
                }

                onDropped: {
                    var file = drop.urls[0].replace("file:///", '')
                    if(drop.x > allLoader.x && drop.x < allLoader.x + allLoader.width &&
                            drop.y > allLoader.y && drop.y < allLoader.y + allLoader.height)
                        device.uploadTypeFirmware(type, file)
                    else
                        device.uploadCardFirmware(slot, file)
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: Theme.indent

                    Loader {
                        id: allLoader
                        visible: active
                        active: dropArea.containsDrag
                        Layout.preferredWidth: Theme.pixelSize * 15; Layout.fillHeight: true

                        sourceComponent: Pane {
                            Material.elevation: drag.x > allLoader.x && drag.x < allLoader.x + allLoader.width &&
                                                drag.y > allLoader.y && drag.y < allLoader.y + allLoader.height ? 5
                                                                                                                : 1
                            Label {
                                anchors.fill: parent
                                wrapMode: Label.WordWrap
                                text: qsTr("Hover here to download for all boards of this type")
                            }
                        }
                    }

                    Pane {
                        id: mainPane
                        Layout.fillWidth: true; Layout.fillHeight: true

                        Material.elevation: dropArea.containsDrag && drag.x > x && drag.x < x + width &&
                                            drag.y > y && drag.y < y + height ? 5
                                                                              : 1

                        Column {
                            width: parent.width
                            spacing: Theme.smallIndent
                            anchors.verticalCenter: parent.verticalCenter

                            Label {
                                text: slot + ". " + SSU.cardTypeToString(type)
                                color: Material.accentColor
                            }

                            Label {
                                text: software  // hardware
                                font.pixelSize: Theme.pixelSize * 0.8
                            }
                        }

                        Button {
                            anchors { right: parent.right; verticalCenter: parent.verticalCenter; }
                            visible: !dropArea.containsDrag

                            text: qsTr("Upload")
                            Material.background: Material.LightBlue
                            Material.foreground: "white"
                            onClicked: {
                                var file = controller.getOpenFileName("*.hex")
                                if(file.length > 0)
                                    farmwareUpdatePopupLoader.open(type, slot, file)
                            }
                        }
                    }   // Pane
                }
            }   // DropArea
        }   // Loader
    }

    Loader {
        id: farmwareUpdatePopupLoader
        x: parent.width / 2 - width / 2;
        active: false
        onLoaded: item.open()

        property int type: 0
        property string slot: ""
        property string file: ""

        function open(type, slot, file) {
            this.type = type
            this.slot = slot
            this.file = file
            active = true;
        }
        function close() { active = false; }

        sourceComponent: Dialogs.SettingsPopup {
            width: Theme.pixelSize * 25
            title: qsTr("Are you sure you want to update the firmware? Update?")
            onClosed: farmwareUpdatePopupLoader.active = false

            contentItem: Row {
                spacing: Theme.middleIndent
                layoutDirection: Qt.RightToLeft

                Button {
                    text: qsTr("Current card")
                    onClicked: {
                        device.uploadCardFirmware(farmwareUpdatePopupLoader.slot, farmwareUpdatePopupLoader.file)
                        farmwareUpdatePopupLoader.close()
                    }
                }

                Button {
                    id: allButton
                    text: qsTr("For all cards")
                    onClicked: {
                        device.uploadTypeFirmware(farmwareUpdatePopupLoader.type, farmwareUpdatePopupLoader.file)
                        farmwareUpdatePopupLoader.close()
                    }
                }
            }
        }
    }
}
