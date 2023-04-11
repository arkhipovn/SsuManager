import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Dialogs 1.0 as Dialogs
import Device 1.0 as D

import Theme 1.0
import SSU 1.0

import "pages/"

D.Panel {
    id: panel
    Material.elevation: 3

    font.family: viewSettings.panelFontFamily
    Material.foreground: viewSettings.panelTitleColor

    readonly property var monitoringCard: device == null ? null
                                                         : device.panel.inventory.monitoringCard

    // Основные платы
    Repeater {
        model: device == null ? null
                              : device.panel.inventory.data

        delegate: Loader {
            id: cardLoader
            x: modelData.address * (panel.slotWidth + 1);
            //            width: panel.slotWidth * modelData.numberOfSeats + (modelData.numberOfSeats - 1)    // (numberOfSeats - 1) - это чтобы убрать пробелы для больших плат
            height: parent.height
            enabled: modelData.type !== SSU.Stub && modelData.type !== SSU.InvalidType

            readonly property var slotConfig: device.panel.configuration.slot(modelData.address)
            readonly property bool validCard: !enabled || slotConfig === null ? true
                                                                              : slotConfig.configuration.contains(modelData.type)

            //@N подумать и переделать. 3 раза проходимся получается по списку. Мб сделать 1 запрос и сразу обе ошибки искать?
            readonly property bool wrongSlot: !enabled || device == null ? false
                                                                         : device.activeEvents.hasData ? device.activeEvents.contains(modelData.slot, Vch003.WrongSlot)
                                                                                                       : false
            readonly property bool noActiveCard: !enabled || device == null ? false
                                                                            : device.activeEvents.hasData ? device.activeEvents.contains(modelData.slot, Vch003.NoActiveCard)
                                                                                                          : false
            readonly property bool cardNotFullyInserted: !enabled || device == null ? false
                                                                                    : device.activeEvents.hasData ? device.activeEvents.contains(modelData.slot, Vch003.CardNotFullyInserted)
                                                                                                                  : false

            opacity: slotConfig !== null && slotConfig.available ? 1.0
                                                                 : 0.7

            Loader {
                active: !validCard || wrongSlot || noActiveCard || cardNotFullyInserted
                z: 2
                sourceComponent: Rectangle {
                    width: cardLoader.width; height: cardLoader.height
                    color: Material.color(Material.Red)
                    opacity: 0.2
                }
            }


            Connections {
                target: cardLoader.item
                onClicked: {
                    if(cardLoader.validCard)
                        panel.openCardPage(modelData.address)
                }
            }

            source: {
                switch(modelData.type) {
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



    // Блоки питания
    Repeater {
        model: monitoringCard === null ? null
                                       : [ monitoringCard.powerA, monitoringCard.powerB ]
        Loader {
            active: modelData.cardActive
            x: modelData.address * (panel.slotWidth + 1); y: index * height
            height: parent.height / 2
            source: "qrc:/vch003/cards/PowerCard.qml"
        }
    }


    property var listOfAddress: []
    property var listOfForms: []

    // Открывает окно с параметрами платы

    function openCardPage(address) {
        var index = panel.listOfAddress.indexOf(address)
        if(index === -1) {
            // такая форма не открыта, можно открывать
            var page = cardPageWindowComponent.createObject(null/*panel*/, { address: address })
            panel.listOfAddress.push(address); panel.listOfAddressChanged()
            panel.listOfForms.push(page); panel.listOfFormsChanged()
        }
        else
            panel.listOfForms[index].requestActivate()
    }

    // Закрывает окно с параметрами платы
    function closePage(address) {
        if(address === -1)
            return
        var index = panel.listOfAddress.indexOf(address)
        if(index === -1) return
        var obj = panel.listOfForms[index]
        panel.listOfForms.splice(index, 1); panel.listOfFormsChanged()
        panel.listOfAddress.splice(index, 1); panel.listOfAddressChanged()
        obj.destroy()
    }

    // Закрывает все окна с параметрами
    function closeAllPages() {
        for(var i = 0; i < panel.listOfForms.length; ++i)
            panel.listOfForms[i].destroy()
        panel.listOfForms = []; panel.listOfFormsChanged()
        panel.listOfAddress = []; panel.listOfAddressChanged()
    }

    // Окно с параметрами платы
    Component {
        id: cardPageWindowComponent

        Dialogs.ApplicationWindow {
            id: cardPageWindow

            property int address: -1
            readonly property var card: device == null || address === -1 ? null
                                                                         : device.panel.inventory.hasData ? device.panel.inventory.card(address)
                                                                                                          : null

            onCardChanged: {
                if(card === null)
                    cardPageWindow.close()
            }

            onClosing: panel.closePage(address)


            settingsCategory: card === null ? ""
                                            : card.slot + "Window"

            readonly property int groupNumber: {
                if(device == null || address === -1)
                    return 0
                var ssuSlot = device.panel.configuration.slot(address)
                return ssuSlot === null ? 0
                                        : ssuSlot.configuration.groupNumber
            }

            Component.onCompleted: {
                //@N как правильно тут сделать?
                // Когда делал привязку была проблема: при обновлении card он изменял размер автоматически
                if(card === null)
                    return

                var w = Math.min(65 * Theme.pixelSize, 1000)
                var h = Math.min(55 * Theme.pixelSize, 1000)

                switch(card.type) {
                case SSU.Input:
                    w = Math.min(48 * Theme.pixelSize, 1000)
                    h = Math.min(35 * Theme.pixelSize, 1000)
                    break
                case SSU.Monitoring:
                    w = Math.min(55 * Theme.pixelSize, 1000)
                    h = Math.min(45 * Theme.pixelSize, 1000)
                    break
                case SSU.Generator:
                    w = Math.min(45 * Theme.pixelSize, 1000)
                    h = Math.min(60 * Theme.pixelSize, 1000)
                    break
                case SSU.Output:
                    w = Math.min(55 * Theme.pixelSize, 1000)
                    h = Math.min(60 * Theme.pixelSize, )
                    break
                case SSU.Distribution:
                    w = Math.min(38 * Theme.pixelSize, 1000)
                    h = Math.min(45 * Theme.pixelSize, 1000)
                    break
                case SSU.Gnss:
                    w = Math.min(70 * Theme.pixelSize, 1000)
                    h = Math.min(50 * Theme.pixelSize, 1000)
                    break
                default: break
                }

                width = settings.value("width", w)
                height = settings.value("height", h)
            }

            title: card === null ? ""
                                 : SSU.cardTypeToString(card.type) + " [" + card.slot + "]"

            Loader {
                width: parent.width; height: parent.height

                sourceComponent: {
                    if(card != null) {
                        switch(card.type) {
                        case SSU.Input: return inputCardDelegate
                        case SSU.Generator: return generatorCardDelegate
                        case SSU.Output: return outputCardDelegate
                        case SSU.Monitoring: return monitoringCardDelegate
                        case SSU.Distribution: return distributionCardDelegate
                        case SSU.Gnss: return gnssCardDelegate
                        default: break
                        }
                    }
                    return invalidCardDelegate
                }
            }

            Component { id: gnssCardDelegate; GnssCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: inputCardDelegate; InputCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: generatorCardDelegate; GeneratorCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: outputCardDelegate; OutputCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: distributionCardDelegate; DistributionCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: monitoringCardDelegate; MonitoringCardPage { device: panel.device; card: cardPageWindow.card; groupNumber: cardPageWindow.groupNumber; } }
            Component { id: invalidCardDelegate; Label { text: qsTr("Invalid card.."); horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter; } }
        }
    }
}
