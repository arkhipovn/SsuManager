import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Menu 1.0 as M
import Device 1.0 as D
import Controls 1.0 as Controls

import Theme 1.0

import SSU 1.0

M.MenuPage {
    id: page

    topPadding: Theme.smallIndent; bottomPadding: 0
    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

    property bool changed: false

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    D.Panel {
        id: panel
        width: parent.width; height: parent.height
        device: page.device
        Material.elevation: 3

        // Основные платы
        Repeater {
            id: repeater
            model: device == null ? null
                                  : device.panel.configuration.data
            delegate: Loader {
                id: cardLoader
                x: slotConfig.address * (panel.slotWidth + 1);
                height: parent.height
                enabled: cardLoader.type !== SSU.Stub && cardLoader.type !== SSU.InvalidType

                readonly property var slotConfig: modelData
                property bool available: false
                property int type: SSU.InvalidType

                Binding {
                    target: cardLoader
                    property: "available"
                    when: !page.changed
                    value: slotConfig.available
                }

                Binding {
                    target: cardLoader
                    property: "type"
                    when: !page.changed
                    value: slotConfig.type
                }

                Connections {
                    target: page.controlRights ? cardLoader.item
                                               : null
                    onClicked: {
                        if(mouse.button === Qt.LeftButton) {
                            cardLoader.available = !cardLoader.available
                            changed = true
                        }
                        else
                            menuLoader.active = true
                    }
                }

                Loader {
                    id: menuLoader
                    active: false
                    sourceComponent: Menu {
                        margins: Theme.indent
                        visible: true
                        modal: true
                        onClosed: menuLoader.active = false
                        title: SSU.cardTypeToString(cardLoader.type)

                        Repeater {
                            model: slotConfig.configuration.types.length

                            MenuItem {
                                text: SSU.cardTypeToString(slotConfig.configuration.types[index])
                                onClicked: {
                                    cardLoader.type = slotConfig.configuration.types[index]
                                    cardLoader.available = true
                                }
                            }
                        }
                    }
                }

                source: {
                    if(!cardLoader.available)
                        return "qrc:/vch003/cards/Stub.qml"
                    switch(cardLoader.type) {
                    case SSU.Input:         return "qrc:/vch003/cards/InputCard.qml"
                    case SSU.Generator:     return "qrc:/vch003/cards/GeneratorCard.qml"
                    case SSU.Output:        return "qrc:/vch003/cards/OutputCard.qml"
                    case SSU.Monitoring:    return "qrc:/vch003/cards/MonitoringCard.qml"
                    case SSU.Distribution:  return "qrc:/vch003/cards/DistributionCard.qml"
                    case SSU.Gnss:          return "qrc:/vch003/cards/GnssCard.qml"
                    default: break
                    }
                    return "qrc:/vch003/cards/MainCard.qml"
                }
            }
        }
    }   // Panel


    footer: Controls.ToolBar {
        Material.elevation: 0

        contentItem: RowLayout {
            spacing: Theme.smallIndent

            Loader {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                active: page.controlRights
                visible: active

                sourceComponent: Label {
                    text: qsTr("Click on card to add or remove from config")
                    wrapMode: Label.WordWrap
                    Material.foreground: Material.accent
                }
            }

            Button {
                text: qsTr("Cancel")
                visible: page.changed
                onClicked: page.changed = false
            }

            Button {
                text: qsTr("Update")
                onClicked: {
                    device.updatePanelConfiguration()
                    page.changed = false
                }
            }

            Loader {
                active: page.controlRights
                visible: active
                sourceComponent: Button {
                    text: qsTr("Apply")
                    onClicked: {
                        var availableCards = []
                        var types = []
                        for(var i = 0; i < repeater.count; ++i) {
                            var item = repeater.itemAt(i)
                            types.push(item.type)
                            availableCards.push(item.available)
                        }

                        device.setPanelConfiguration(availableCards, types)
                        page.changed = false
                    }
                }
            }
        }   // RowLayout
    }   // ToolBar


}
