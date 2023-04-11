import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import SSU 1.0
import Theme 1.0
import Strings 1.0
import Models 1.0

import "private/"

CardPage {
    id: page

    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..

    header: Pane {
        spacing: 0
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent;

        contentItem: RowLayout {
            id: headerContent
            spacing: 0

            property bool shortContent: false

            Controls.Button {
                id: activeSlotButton
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                visible: card.activeChannel !== 255 && card.pllState !== SSU.PllState.Warmup
                symbol: device == null ? ""
                                       : device.channelDescription.cardInputChannelSlot(card.type, card.activeChannel)
                size: Theme.pixelSize * 0.9

                textLabel.visible: !headerContent.shortContent

                text: qsTr("Slot")
                clickable: page.controlRights

                backgroundColor: Material.color(Material.LightGreen)
                toolTip.text: card === null ? ""
                                            : qsTr("Active slot") + ": " + activeSlotButton.symbol

                Controls.ToolTip {
                    x: activeSlotButton.toolTip.x; y: activeSlotButton.toolTip.y + activeSlotButton.toolTip.height + Theme.smallIndent
                    visible: activeSlotButton.hovered && activeSlotButton.clickable
                    delay: 600
                    text: qsTr("Click to change active card")
                }

                onClicked: changeActiveSlotPopup.open()

                Dialogs.ConfirmationPopup {
                    id: changeActiveSlotPopup
                    text: qsTr("Are you sure you want to change the active card?")
                    onClicked: device.send(card.commandToChangeActiveSlot())
                }
            }

            Controls.Button {
                id: activeChannelButton
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                visible: card.activeChannel !== 255 && card.pllState !== SSU.PllState.Warmup
                symbol: device == null ? ""
                                       : device.channelDescription.cardInputChannelToString(card.type, card.activeChannel, ChannelDescription.NumberOnly)
                size: Theme.pixelSize * 0.9

                textLabel.visible: !headerContent.shortContent

                text: qsTr("Channel")
                clickable: false

                backgroundColor: Material.color(Material.LightGreen)

                toolTip.visible: activeChannelButton.hovered && card.activeChannel !== 255
                toolTip.text: card === null || device === null ? ""
                                                               : qsTr("Active channel") + ": " + device.channelDescription.cardInputChannelToString(card.type, card.activeChannel)
            }

            Controls.Button {
                id: workingModeButton
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                text: qsTr("Mode")
                clickable: page.controlRights

                textLabel.visible: !headerContent.shortContent

                symbol: {
                    if(card === null) return ""
                    switch(card.mode) {
                    case SSU.WorkingMode.AutoMode:          return "A"
                    case SSU.WorkingMode.ManualMode:        return "M"
                    case SSU.WorkingMode.ForcedHoldover:    return "H"
                    default: return ""
                    }
                }

                toolTip.text: qsTr("Mode") + ": " + SSU.workingModeToString(card === null ? 0 : card.mode)

                Controls.ToolTip {
                    x: workingModeButton.toolTip.x; y: workingModeButton.toolTip.y + workingModeButton.toolTip.height + Theme.smallIndent
                    visible: workingModeButton.hovered && workingModeButton.clickable
                    delay: 600
                    text: qsTr("Click to set mode")
                }

                onClicked: controlPopup.openPopup(workingModeSettingsComponent, -1, qsTr("Generator mode"))
            }

            Controls.Button {
                id: ssmButton
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                clickable: false
                text: qsTr("SSM")
                textLabel.visible: !headerContent.shortContent
                symbol: SSU.ssmToString(card.ssm)
                toolTip.text: qsTr("SSM") + ": " + symbol
            }

            Item {
                Layout.fillWidth: true

                function calcWidth() {
                    if(width <= 0)
                        headerContent.shortContent = true
                    else {
                        var butListWidth = (workingModeButton.visible ? workingModeButton.textLabel.width
                                                                      : 0) +
                                (activeChannelButton.visible ? activeChannelButton.textLabel.width
                                                             : 0) +
                                (activeSlotButton.visible ? activeSlotButton.textLabel.width
                                                          : 0) +
                                (ssmButton.visible ? ssmButton.textLabel.width
                                                   : 0)
                        if(width > butListWidth * 1.2)
                            headerContent.shortContent = false
                    }
                }

                Component.onCompleted: calcWidth()
                onWidthChanged: calcWidth()
            }

            Loader {
                active: card.error !== 0
                visible: active
                sourceComponent: QCC.SymbolButton {
                    topInset: 0; bottomInset: 0
                    symbol: "\uf05a"
                    squareBackground: false
                    clickable: false
                    color: Material.color(Material.Red)
                    toolTip.text: Vch003.generatorErrorToStringList(card.error).join("\n");
                }
            }

            Loader {
                active: page.controlRights
                visible: active
                sourceComponent: QCC.SymbolButton {  // settings
                    topInset: 0; bottomInset: 0
                    symbol: "\uf013"
                    squareBackground: false
                    highlighted: generatorSettingsDialogLoader.active
                    onClicked: generatorSettingsDialogLoader.active = true
                }
            }

            PllStateItem {
                id: pllStateButton
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                state: card === null ? 0 : card.pllState
                title: card === null || card.pllStateTimer === 0 ? ""
                                                                 : Strings.minToDateString(card.pllStateTimer)

                readonly property bool toolTipVisible: card !== null && card.mode === SSU.WorkingMode.AutoMode &&
                                                       (card.pllState === SSU.PllState.Freerun || card.pllState === SSU.PllState.Holdover)

                toolTip.timeout: 10000
                toolTip.visible: toolTipVisible && pllStateButton.hovered
                toolTip.contentItem: Label {    // Чтобы сделать перенос у текста..
                    wrapMode: Label.WordWrap
                    text: pllStateButton.toolTip.text
                }

                toolTip.text: qsTr("With automatic control the generator can be in mode in") + " <b>" +
                              SSU.pllStateToString(SSU.PllState.Holdover) + "</b>" + qsTr(" or ") + "<b>" +
                              SSU.pllStateToString(SSU.PllState.Freerun) + "</b>" + qsTr(" the following cases:") +
                              "<p> - " + qsTr("no active input signals") +
                              "<p> - " + qsTr("the quality of the active signal is below") +
                              " <b>" + SSU.ssmToString(SSU.SSU_T) + "</b>"

                Controls.ToolTip {
                    x: pllStateButton.toolTip.x; y: pllStateButton.toolTip.y + pllStateButton.toolTip.height + Theme.smallIndent
                    visible: pllStateButton.hovered && pllStateButton.title.length > 0

                    text: {
                        if(pllStateButton.state === SSU.PllState.Tracking)
                            return qsTr("Ability to switch to forced hold mode through") + " " + pllStateButton.title
                        else {
                            var str = qsTr("Switch to mode") + " <b>"

                            switch(pllStateButton.state) {
                            case SSU.PllState.Warmup:
                                str += SSU.pllStateToString(SSU.PllState.Adjustment)
                                break
                            case SSU.PllState.Adjustment:
                                str += SSU.pllStateToString(SSU.PllState.Tracking)
                                break
                            case SSU.PllState.Holdover:
                                str += SSU.pllStateToString(SSU.PllState.Freerun)
                                break
                            default:
                                return ""
                            }

                            return str + "</b> " + qsTr("through") + " " + pllStateButton.title
                        }
                    }
                }
            }   // PllStateItem
        }
    }

    footerData: [
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: card.isMaster ? SSU.controlModeToString(SSU.Master)
                                : SSU.controlModeToString(SSU.Slave)
            clickable: false
            toolTip.text: card.isMaster ? SSU.controlModeToString(SSU.Master)
                                        : SSU.controlModeToString(SSU.Slave) +
                                          ". " + qsTr("Reads parameters from Master")
        },
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: SSU.generatorTypeToString(card.generatorType)
            clickable: false
            toolTip.text: qsTr("Generator type") + ": " + text
        },
        Controls.Button {
            id: ssuTypeButton
            Layout.alignment: Qt.AlignVCenter
            symbol: card.ssuType === SSU.SsuType.SSU_A ? qsTr("SSU A") : qsTr("SSU B")
            toolTip.text: qsTr("SSU type") + ": " + symbol

            clickable: page.controlRights

            Controls.ToolTip {
                x: ssuTypeButton.toolTip.x; y: ssuTypeButton.toolTip.y + ssuTypeButton.toolTip.height + Theme.smallIndent
                visible: ssuTypeButton.hovered && ssuTypeButton.clickable
                delay: 600
                text: qsTr("Click to set ssu type")
            }

            onClicked: controlPopup.openPopup(ssuTypeSettingsComponent, -1, qsTr("SSU type"))

            //            setSsuTypePasswordPopup.open()
            //            Dialogs.CheckPasswordPopup {
            //                id: setSsuTypePasswordPopup
            //                title: qsTr("Are you sure you want to reset the settings to default?")
            //                text: qsTr("To confirm the action, enter the password again")
            //                applyButtonText: qsTr("Restore")
            //                password: device == null ? ""
            //                                         : device.connectionSettings.password
            //                onClicked: device.resetToDefaultCard(card.slot)
            //            }
        }
    ]

    ListView {
        //        clip: true
        width: parent.width; height: parent.height
        spacing: Theme.middleIndent

        model: [    //@N Пока что сделаю так.. Не знаю как правильно лучше..
            [
                {
                    slot: 0,
                    numberOfSsuChannels:   [ 0, 1, 2, 3 ],
                    numberOfCardInputs:    [ 0, 1, 2, 3 ]
                },
                {
                    slot: 1,
                    numberOfSsuChannels:   [ 0, 1, 2, 3 ],
                    numberOfCardInputs:    [ 4, 5, 6, 7 ]
                }
            ],
            [
                {
                    slot: 2,
                    numberOfSsuChannels:   [ 4, 5, 6, 7 ],
                    numberOfCardInputs:    [ 8, 9, 10, 11 ]
                },
                {
                    slot: 3,
                    numberOfSsuChannels:   [ 4, 5, 6, 7 ],
                    numberOfCardInputs:    [ 12, 13, 14, 15 ]
                }
            ],
            [
                {
                    slot: 4,
                    numberOfSsuChannels:   [ 8 ],
                    numberOfCardInputs:    [ 16 ]
                }
            ],
            [
                {
                    slot: 5,
                    numberOfSsuChannels:   [ 9 ],
                    numberOfCardInputs:    [ 17 ]
                }
            ]
        ]

        boundsBehavior: ListView.StopAtBounds
        flickableDirection: ListView.VerticalFlick
        headerPositioning: ListView.OverlayHeader
        delegate: inputGroupDelegate
    }

    Component {
        id: inputGroupDelegate

        RowLayout {
            id: inputGroupRow
            width: ListView.view.width;

            readonly property var group: modelData

            // Нужно, чтобы взять SSM
            readonly property var inputCard: {
                if(device == null)
                    return null
                device.panel.inventory.hasData
                for(let i = 0; i < group.length; ++i) {
                    var card = device.panel.inventory.card(group[i].slot)
                    if(card !== null) {
                        return card
                    }
                }
                return null
            }

            Repeater {
                model: group
                delegate: Controls.GroupBox {
                    Layout.fillWidth: true;
                    title: SSU.addressToString(modelData.slot)
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent

                    contentItem: Column {
                        spacing: Theme.smallIndent

                        Repeater {
                            model: modelData.numberOfCardInputs

                            delegate: ChannelStatusLabel {
                                id: channelStatusLabel
                                readonly property var channel: card.inputChannels[modelData]

                                priority: channel.currentPriority + 1
                                text: device == null ? ""
                                                     : device.channelDescription.cardInputChannelToString(card.type, modelData, ChannelDescription.Short)
                                backgroundColor: channel.ban ? Material.color(Material.Red)
                                                             : Material.color(Material.LightGreen)

                                toolTip.text: {
                                    if(device == null)
                                        return ""
                                    var name = device.channelDescription.cardInputChannelToString(card.type, modelData)
                                    return name + " - " +
                                            (channel.ban ? qsTr("Ban") : qsTr("OK")) + "\n" +
                                            (activeIconLoader.active ? (qsTr("Active") + "\n")
                                                                     : "") +
                                            (qsTr("Current priority") + ": " + channelStatusLabel.priority )
                                }

                                ActiveIconLoader {
                                    id: activeIconLoader
                                    active: {
                                        switch(card.pllState) {
                                        case SSU.PllState.Warmup:
                                        case SSU.PllState.Freerun:
                                        case SSU.PllState.Holdover:
                                            return false
                                        default:
                                            return modelData === card.activeChannel
                                        }
                                    }
                                }   // ActiveIconLoader
                            }   // delegate

                        }   // Repeater
                    }   // Column
                }   // Group box
            }   // Repeater group


            // Info group box
            Controls.GroupBox {
                id: paramGroupBox
                Layout.fillWidth: true

                label: Controls.TableHeader {
                    topPadding: 0; bottomPadding: 0
                    leftPadding: Theme.smallIndent; rightPadding: Theme.smallIndent
                    spacing: Theme.smallIndent
                    implicitHeight: implicitContentHeight
                    width: paramGroupBox.width

                    Controls.TableCell {
                        Layout.preferredWidth: 1
                        Layout.fillWidth: true; Layout.fillHeight: true
                        text: qsTr("Description")
                    }

                    Controls.TableCell {
                        Layout.preferredWidth: 1
                        Layout.fillWidth: true; Layout.fillHeight: true
                        text: qsTr("Custom priority")
                    }

                    Controls.TableCell {
                        Layout.preferredWidth: 1
                        Layout.fillWidth: true; Layout.fillHeight: true
                        text: qsTr("SSM")
                    }
                }   // GroupBox header

                contentItem: Column {
                    spacing: Theme.smallIndent

                    Repeater {
                        model: group[0].numberOfCardInputs

                        delegate: ChannelPane {
                            id: channelPane
                            width: parent.width

                            readonly property int numberOfSsuInput: inputGroupRow.group[0].numberOfSsuChannels[index]
                            readonly property int numberOfCardInput: modelData
                            readonly property var channel: card.inputChannels[numberOfCardInput]

                            Controls.ValueLabel {
                                Layout.fillWidth: true; Layout.fillHeight: true
                                Layout.preferredWidth: 1
                                horizontalAlignment: Label.AlignLeft

                                text: {
                                    if(device == null)
                                        return ""
                                    device.channelDescription.hasTag
                                    return device.channelDescription.ssuInputChannelTag(channelPane.numberOfSsuInput)
                                }

                                clickable: true
                                toolTip.text: qsTr("Click to set channel description")
                                onClicked: controlPopup.openPopup(descriptionSettingsComponent, channelPane.numberOfSsuInput,
                                                                  device.channelDescription.ssuInputChannelName(channelPane.numberOfSsuInput) + ". " + qsTr("Channel description"))
                            }

                            Controls.ValueLabel {
                                Layout.fillWidth: true; Layout.fillHeight: true
                                Layout.preferredWidth: 1
                                horizontalAlignment: Label.AlignLeft
                                text: channel.priority + 1  // +1 чтобы отображалось начиная с 1
                                clickable: page.controlRights && card.mode === SSU.WorkingMode.AutoMode

                                toolTip.text: qsTr("Click to set priority")
                                onClicked: controlPopup.openPopup(prioritySettingsComponent, channelPane.numberOfCardInput,
                                                                  device.channelDescription.ssuInputChannelName(channelPane.numberOfSsuInput) + ". " + qsTr("Priority"))
                            }

                            // SSM
                            Controls.ValueLabel {
                                Layout.fillWidth: true; Layout.fillHeight: true
                                Layout.preferredWidth: 1
                                horizontalAlignment: Label.AlignLeft
                                clickable: false

                                function getInputSsm(index) {
                                    if(inputGroupRow.inputCard === null)
                                        return SSU.SSM.None

                                    switch(inputGroupRow.inputCard.type) {
                                    case SSU.Input:
                                        if(inputGroupRow.inputCard.inputChannels !== null &&
                                                inputGroupRow.inputCard.inputChannels.length > index)
                                        {
                                            return inputGroupRow.inputCard.inputChannels[index].ssm
                                        }
                                        break
                                    case SSU.Gnss:
                                        return inputGroupRow.inputCard.ssm
                                    default: break
                                    }
                                    return SSU.SSM.None
                                }

                                text: SSU.ssmToString(getInputSsm(index))
                            }
                        }   // ChannelPane

                    }    // Info group Repeater
                }   // Info group Column
            }    // Info groupbox
        }   // inputGroupRow Layout
    }   // inputGroupDelegate Component



    Dialogs.SettingsPopup {
        id: controlPopup

        function openPopup(component, numberOfChannel, title) {
            controlPopup.title = title
            loader.numberOfChannel = numberOfChannel
            loader.sourceComponent = component
            open()
        }

        onClosed: loader.sourceComponent = null

        contentItem: Loader {
            id: loader
            property int numberOfChannel: -1
            sourceComponent: null
        }
    }


    Component {
        id: descriptionSettingsComponent

        Column {
            TextField {
                id: textField
                width: parent.width
                focus: true
                selectByMouse: true
                wrapMode: TextField.Wrap

                text: {
                    if(device == null)
                        return ""
                    device.channelDescription.hasTag
                    return device.channelDescription.ssuInputChannelTag(numberOfChannel)
                }
            }

            Button {
                anchors.right: parent.right;
                text: qsTr("Apply")
                onClicked: {
                    device.setTag(numberOfChannel, textField.text)
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: prioritySettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.inputChannels[numberOfChannel].priority
                model: device == null ? null
                                      : device.channelDescription.numberOfSsuInputChannels()
                delegateTextFunc: function(value) { return value + 1; }
            }

            Label {
                topPadding: Theme.middleIndent; bottomPadding: Theme.middleIndent
                width: parent.width
                wrapMode: Label.WordWrap
                text: qsTr("Priorities are configured immediately for the output and the generator card")
                color: Material.color(Material.Orange)
            }

            Button {
                anchors.right: parent.right
                enabled: comboBox.value !== card.inputChannels[numberOfChannel].priority
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetChannelPriority(numberOfChannel, comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }

    Component {
        id: ssuTypeSettingsComponent

        Column {
            spacing: 0

            readonly property int ssuType: ssuAMode.checked ? SSU.SSU_A :
                                                              SSU.SSU_B

            RadioButton {
                id: ssuAMode
                width: parent.width
                checked: card.ssuType ===  SSU.SSU_A
                text: qsTr("SSU A")
            }

            RadioButton {
                id: ssuBButton
                width: parent.width
                checked: card.ssuType === SSU.SSU_B
                text: qsTr("SSU B")
            }

            Button {
                anchors.right: parent.right
                enabled: card.ssuType !== ssuType
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetSsuType(ssuType))
                    controlPopup.close()
                }
            }
        }
    }   // component

    Component {
        id: workingModeSettingsComponent

        Column {
            spacing: 0

            readonly property int newWorkingMode: autoMode.checked ? SSU.WorkingMode.AutoMode :
                                                                     manualButton.checked ? SSU.WorkingMode.ManualMode :
                                                                                            SSU.WorkingMode.ForcedHoldover

            RadioButton {
                id: autoMode
                width: parent.width
                //                topInset: 0; bottomInset: 0
                checked: card.mode ===  SSU.WorkingMode.AutoMode
                text: SSU.workingModeToString(SSU.WorkingMode.AutoMode)
            }

            RadioButton {
                id: manualButton
                width: parent.width
                //                topInset: 0; bottomInset: 0
                checked: card.mode === SSU.WorkingMode.ManualMode
                text: SSU.workingModeToString(SSU.WorkingMode.ManualMode)
            }

            Controls.ComboBox {
                id: manualChannelComboBox
                topInset: 0; bottomInset: 0
                width: parent.width
                visible: manualButton.checked
                model: device.options.kNumberOfGeneratorInChannels
                value: card.manualChannel
                delegateTextFunc: function(index) { return device == null ? ""
                                                                          : device.channelDescription.cardInputChannelToString(card.type, index) }
            }

            RadioButton {
                id: holdoverMode
                width: parent.width
                //                topInset: 0; bottomInset: 0
                checked: card.mode === SSU.WorkingMode.ForcedHoldover
                text: SSU.workingModeToString(SSU.WorkingMode.ForcedHoldover)
            }

            Button {
                anchors.right: parent.right
                enabled: card.mode !== newWorkingMode || card.manualChannel !== manualChannelComboBox.value
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetWorkingMode(newWorkingMode, manualChannelComboBox.value))
                    controlPopup.close()
                }
            }
        }
    }   // component

    Loader {
        id: generatorSettingsDialogLoader
        active: false
        sourceComponent: Dialogs.ModalDialog {
            id: generatorSettingsDialog
            title: qsTr("Settings")
            width: Math.min(Theme.pixelSize * 30, 500); height: Math.min(40 * Theme.pixelSize, 1000)
            onClosing: generatorSettingsDialogLoader.active = false

            property bool changed: false

            ColumnLayout {
                anchors { left: parent.left; right: parent.right; margins: Theme.indent; }
                //                spacing: Theme.middleIndent
                enabled: page.controlRights

                CheckBox {
                    id: ssmTableCheckBox
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth
                    topInset: 0; bottomInset: 0
                    text: qsTr("Enable SSM table")

                    Binding {
                        when: !generatorSettingsDialog.changed
                        target: ssmTableCheckBox
                        property: "checked"
                        value: card.enableSSMTable
                    }

                    onClicked: generatorSettingsDialog.changed = true
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Label.WordWrap
                    text: qsTr("When disabled, signal selection for capture will only be made based on user priorities regardless of SSM")
                    color: Material.shade(Material.foreground, Material.Shade300)
                }
            }

            footer: Controls.ToolBar {
                Material.elevation: 0
                visible: page.controlRights
                leftPadding: Theme.indent; rightPadding: Theme.indent

                Row {
                    anchors.right: parent.right
                    spacing: Theme.middleIndent

                    Button {
                        text: qsTr("Cancel")
                        visible: page.controlRights && applyButton.enabled
                        onClicked: generatorSettingsDialog.changed = false
                    }

                    Button {
                        id: applyButton
                        enabled: generatorSettingsDialog.changed
                        text: qsTr("Apply")
                        onClicked: {
                            if(ssmTableCheckBox.checked !== card.enableSSMTable)
                                device.send(card.enableSSMTableCommand(ssmTableCheckBox.checked))
                            //                            if(priTableSyncCheckBox.checked !== card.enableCopyPriority)
                            //                                device.send(card.enableCopyPriorityCommand(priTableSyncCheckBox.checked))
                            generatorSettingsDialog.changed = false
                        }
                    }
                }   // Row
            }   // ToolBar
        }
    }

}
