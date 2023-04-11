import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12

import QtQuick.Layouts 1.12

import Controls 1.0 as Controls
import Dialogs 1.0 as Dialogs

import QCC.Controls 1.0 as QCC

import SSU 1.0
import Theme 1.0
import Models 1.0

import "private/"

CardPage {
    id: page

    footerData: [
        // Идикация переключения генератора (изменение активного сигнала)
        Loader {
            id: activeChannelSwitchingIndicatorLoader
            Layout.alignment: Qt.AlignVCenter
            active: device == null ? false
                                   : device.activeEvents.hasData ? device.activeEvents.contains(card.slot, Vch003.DeviceEvent.ExecCmdChanSwitch)
                                                                 : false
            sourceComponent: QCC.SymbolButton {
                clickable: false
                text: qsTr("Switching")
                symbol: "\uf110"
                color: Material.color(Material.Orange)
                toolTip.text: qsTr("Switching the active channel")
            }
        },
        QCC.SymbolButton {
            Layout.alignment: Qt.AlignVCenter
            text: card.active ? qsTr("Active", "CardMode") : qsTr("Passive")

            clickable: {
                if(device == null || !card.active || !card.synchronization)
                return false

                device.panel.inventory.hasData
                let list = device.panel.getCardGroup(groupNumber, card.type)
                let count = 0;
                for(let i = 0; i < list.length; ++i) {
                    if(list[i].synchronization)
                    count++
                }
                return count > 1
            }

            toolTip.text: !card.active ? ""
                                       : clickable ? qsTr("Click to give activity")
                                                   : !card.active ? qsTr("Passive")
                                                                  : !card.synchronization ? qsTr("Synchronization error. Unable to release activity")
                                                                                          : qsTr("Passive card not available")
            textColor: Material.accentColor

            onClicked: giveActivityConfirmPopup.open()

            Dialogs.ConfirmationPopup {
                id: giveActivityConfirmPopup
                text: qsTr("Are you sure you want to change the active card?")
                onClicked: device.send(card.commandToGiveActivity())
            }
        }
    ]


    readonly property bool controlRights: device == null ? false
                                                         : device.options.userRights.parameterControl(device.connectionSettings.level) //@N Не знаю.. Мб переделать все это..


    // Плата генератора
    readonly property var generatorCard: device == null ? null
                                                        : device.panel.inventory.hasData ? device.panel.inventory.findCard(SSU.Generator)
                                                                                         : null


    header: Pane {
        spacing: 0
        leftPadding: Theme.indent; rightPadding: Theme.indent
        topPadding: Theme.smallIndent; bottomPadding: Theme.smallIndent;

        contentItem: RowLayout {
            id: headerContent
            spacing: 0

            property bool shortContent: true

            Controls.Button {
                id: prioritiesButton
                textLabel.visible: !headerContent.shortContent
                text: qsTr("Priorities in bypass mode")
                symbol: "\uf160"
                toolTip.text: qsTr("Click to set priorities of input signals in bypass mode")
                onClicked: prioritiesSettingsPopupLoader.active = true
            }

            Loader {
                id: changeActiveChannelButtonLoader
                active: page.controlRights && card.synchronization && card.activeChannel !== card.backupChannel

                visible: active

                sourceComponent: Controls.Button {
                    symbol: "\uf074"
                    text: qsTr("Switch to backup")
                    textLabel.visible: !headerContent.shortContent
                    toolTip.text: qsTr("Click to switch to backup channel")
                    //                onClicked: prioritiesMenuLoader.open()
                    enabled: !activeChannelSwitchingIndicatorLoader.active
                    onClicked: switchActiveChannelConfirmPopup.open()

                    Dialogs.ConfirmationPopup {
                        id: switchActiveChannelConfirmPopup
                        text: qsTr("Are you sure you want to change the active generator?")
                        onClicked: device.send(card.commandToSwitchToBackupGenerator())
                    }
                }
            }

            Item {
                Layout.fillWidth: true

                function calcWidth() {
                    if(width <= 0)
                        headerContent.shortContent = true
                    else {
                        var butListWidth = (prioritiesButton.visible ? prioritiesButton.textLabel.width
                                                                     : 0) +
                                (changeActiveChannelButtonLoader.active && changeActiveChannelButtonLoader.status === Loader.Ready ?
                                     changeActiveChannelButtonLoader.item.textLabel.width
                                   : 0) /*+
                                (activeChannelButton.visible ? activeChannelButton.textLabel.width
                                                             : 0) +
                                (activeSlotButton.visible ? activeSlotButton.textLabel.width
                                                          : 0)*/
                        if(width > butListWidth * 1.2)
                            headerContent.shortContent = false
                    }
                }

                Component.onCompleted: calcWidth()
                onWidthChanged: calcWidth()
            }

            Loader {
                active: !card.synchronization
                visible: active

                sourceComponent: Label {
                    text: qsTr("Sync 500 Hz Failure")
                    color: Material.color(Material.Red)
                }
            }

            Loader {
                active: page.controlRights
                visible: active

                sourceComponent: QCC.SymbolButton {  // settings
                    topInset: 0; bottomInset: 0
                    symbol: "\uf013"
                    squareBackground: false
                    highlighted: outputCardSettingsDialogLoader.active
                    onClicked: outputCardSettingsDialogLoader.active = true
                }
            }

            Label {
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                text: qsTr("Group") + " " + (groupNumber + 1)
                font.bold: true
                color: Material.accentColor
            }
        }
    }   // header Pane


    RowLayout {
        width: parent.width; height: parent.height
        spacing: Theme.middleIndent

        ListView {
            id: inputListView
            Layout.fillHeight: true
            spacing: Theme.middleIndent

            model: [    //@N Пока что сделаю так.. Не знаю как правильно лучше..
                [
                    {
                        slot: 6,
                        numberOfSsuChannels:    [ -1 ],
                        numberOfCardInputs:    [ 18 ]
                    },
                    {
                        slot: 8,
                        numberOfSsuChannels:   [ -1 ],
                        numberOfCardInputs:    [ 19 ]
                    }
                ],
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
                    },
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

        Controls.GroupBox {
            id: outputParamGroupBox
            Layout.fillWidth: true; Layout.fillHeight: true
            title: qsTr("Output channels")

            Flickable {
                width: parent.width; height: parent.height
                boundsBehavior: ListView.StopAtBounds
                flickableDirection: ListView.VerticalFlick
                contentHeight: grid.height

                clip: true

                GridLayout {
                    id: grid
                    clip: true
                    width: parent.width
                    flow: GridLayout.TopToBottom
                    rows: 10
                    rowSpacing: Theme.smallIndent; columnSpacing: Theme.smallIndent

                    Repeater {
                        model: card.outputChannels
                        delegate: outputChannelDelegate
                    }
                }
            }
        }   // Output GroupBox
    }   // RowLayout


    Component {
        id: outputChannelDelegate

        ChannelPane {
            id: channelPane

            Layout.fillWidth: true;
            spacing: 0

            readonly property int numberOfChannel: index
            readonly property var channel: modelData

            ChannelStatusLabel {
                id: channelStateButton
                Layout.preferredWidth: Theme.textFieldWidth
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                text: device == null ? ""
                                     : device.channelDescription.cardOutputChannelToString(card.type, numberOfChannel, ChannelDescription.Short)
                backgroundColor: channel.type === SSU.SignalType.Off ? Material.color(Material.Grey) :
                                                                       Material.color(Material.LightGreen)
                toolTip.text: channelStateButton.text +
                              "<p>" + channelTypeToolTip.text //SSU.signalTypeToString(channel.type)
            }

            Label {
                text: channel.holdoverModeEnabled ? "\uf14a" : "\uf096"
                font.pixelSize: Theme.pixelSize * 1.5
                leftPadding: Theme.smallIndent * 0.5; rightPadding: Theme.smallIndent * 0.5
                color: Material.backgroundDimColor

                background: MouseArea {
                    id: mouseArea
                    hoverEnabled: true
                    onClicked: if(page.controlRights) permOnHoldConfirmPopup.open()
                }

                Controls.ToolTip {
                    id: holdoverModeEnabledToolTip
                    visible: mouseArea.containsMouse
                    text: qsTr("Permission on holdover")
                }

                Controls.ToolTip {
                    x: holdoverModeEnabledToolTip.x; y: holdoverModeEnabledToolTip.y + holdoverModeEnabledToolTip.height + Theme.smallIndent
                    visible: mouseArea.containsMouse && page.controlRights
                    delay: 600
                    text: qsTr("Click to change permission on holdover")
                }

                Dialogs.ConfirmationPopup {
                    id: permOnHoldConfirmPopup
                    text: qsTr("Are you sure you want to change permission on holdover?")
                    onClicked: device.send(card.commandToSetPermissionOnHoldover(index, !channel.holdoverModeEnabled))
                }
            }

            ItemDelegate {
                id: channelTypeItem
                Layout.fillWidth: true; Layout.fillHeight: true
                padding: 0
                topPadding: 0; bottomPadding: 0
                topInset: 0; bottomInset: 0

                property bool clickable: page.controlRights
                //                property bool clickable: page.controlRights && (generatorCard !== null ? true
                //                                                                                       : card.enableBypass)

                contentItem: Item {
                    ColumnLayout {
                        width: parent.width;
                        spacing: 0
                        anchors.verticalCenter: parent.verticalCenter

                        Controls.TableCell {
                            Layout.fillWidth: true
                            text: SSU.signalTypeToString(channel.type)
                            font.pixelSize: Theme.pixelSize * 0.8
                        }

                        QCC.ArrowItem {
                            Layout.fillWidth: true; implicitHeight: Theme.rowHeight * 0.4
                            lineColor: Material.color(Material.BlueGrey)
                            indicatorForm: channel.state ? QCC.ArrowItem.Arrow
                                                         : QCC.ArrowItem.Cross
                        }
                    }
                }

                Controls.ToolTip {
                    id: channelTypeToolTip
                    visible: channelTypeItem.hovered
                    text: SSU.signalTypeToString(channel.type) +
                          (channel.state ? "" : "<p>" + qsTr("No output signal"))
                }

                Controls.ToolTip {
                    x: channelTypeToolTip.x; y: channelTypeToolTip.y + channelTypeToolTip.height + Theme.smallIndent
                    visible: channelTypeItem.hovered && channelTypeItem.clickable
                    delay: 600
                    text: qsTr("Click to set signal type")
                }

                onClicked: {
                    if(channelTypeItem.clickable) controlPopup.openPopup(channelTypeSettingsComponent, channelPane.numberOfChannel,
                                                                         channelStateButton.text + ". " + qsTr("Signal type"))
                }
            }
        }

    }   // outputChannelDelegate


    Component {
        id: inputGroupDelegate

        RowLayout {
            id: inputGroupRow
            //            width: ListView.view.width;
            readonly property var group: modelData

            onWidthChanged: if(index == 0) inputListView.implicitWidth = width

            Repeater {
                model: group
                delegate: Controls.GroupBox {
                    //                    Layout.fillWidth: true;
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

                                clickable: false

                                toolTip.text: {
                                    if(device == null)
                                        return ""
                                    var name = device.channelDescription.cardInputChannelToString(card.type, modelData)
                                    return name + " - " +
                                            (channel.ban ? qsTr("Ban") : qsTr("OK")) + "\n" +
                                            (modelData === card.activeChannel ? (qsTr("Active") + "\n")
                                                                              : modelData === card.backupChannel ? (qsTr("Backup") + "\n")
                                                                                                                 : "") +
                                            (qsTr("Current priority") + ": " + channelStatusLabel.priority)
                                }

                                ActiveIconLoader {
                                    id: activeIconLoader
                                    symbol: modelData === card.activeChannel ? "A"
                                                                             : modelData === card.backupChannel ? "B"
                                                                                                                : ""
                                    active: symbol.length != 0
                                }   // ActiveIconLoader
                            }   // delegate
                        }   // Repeater
                    }   // Column
                }   // Group box
            }   // Repeater group

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }   // inputGroupDelegate


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
    }   // SettingsPopup


    Loader {
        id: prioritiesSettingsPopupLoader
        width: parent.width; height: parent.height
        active: false
        onLoaded: item.open()

        sourceComponent: Dialogs.SettingsPopup {
            id: settingsPopup
            width: prioritiesSettingsPopupLoader.width; height: prioritiesSettingsPopupLoader.height
            title: qsTr("Priorities in bypass mode")
            onClosed: prioritiesSettingsPopupLoader.active = false
            bottomPadding: 0

            property bool changed: false

            ColumnLayout {
                width: parent.width; height: parent.height
                spacing: 0// Theme.smallIndent

                PrioritySettingsList {
                    id: prioritySettingsList
                    interactive: height < contentHeight
                    ScrollBar.vertical: ScrollBar { id: scrollbar; policy: ScrollBar.AlwaysOn; visible: prioritySettingsList.interactive; }
                    contentWidth: scrollbar.visible ? width - scrollbar.width
                                                    : width
                    //                    readOnly: !(page.controlRights && (generatorCard === null ? false
                    //                                                                              : !priTableSyncCheckBox.checked))
                    readOnly: !page.controlRights

                    Layout.fillWidth: true; Layout.fillHeight: true
                    clip: true
                    //                    model: inputListView.model
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

                    device: page.device
                    card: page.card
                }

                Label {
                    visible: generatorCard !== null
                    topPadding: Theme.middleIndent; bottomPadding: Theme.middleIndent
                    Layout.fillWidth: true
                    wrapMode: Label.WordWrap
                    text: qsTr("Priorities are configured immediately for the output and the generator card")
                    color: Material.color(Material.Orange)
                }

                Controls.ToolBar {
                    id: toolBar
                    Layout.fillWidth: true
                    leftInset: 3; rightInset: 3
                    Material.elevation: 0
                    leftPadding: 0; rightPadding: 0

                    RowLayout {
                        width: parent.width
                        spacing: Theme.middleIndent

                        CheckBox {
                            id: ssmTableCheckBox
                            Layout.fillWidth: true
                            Layout.maximumWidth: implicitWidth
                            topInset: 0; bottomInset: 0
                            text: qsTr("Enable SSM table")
                            enabled: page.controlRights

                            Binding {
                                target: ssmTableCheckBox
                                when: !settingsPopup.changed
                                property: "checked"
                                value: card.enableSSMTable
                            }

                            onClicked: settingsPopup.changed = true

                            Controls.ToolTip {
                                visible: ssmTableCheckBox.hovered
                                text: qsTr("When disabled, signal selection for capture will only be made based on user priorities regardless of SSM")
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Button {
                            Layout.fillWidth: true
                            Layout.maximumWidth: implicitWidth

                            text: qsTr("Cancel")
                            visible: page.controlRights && applyButton.enabled
                            onClicked: {
                                settingsPopup.changed = false
                                prioritySettingsList.changed = false
                            }
                        }

                        Button {
                            id: applyButton
                            Layout.fillWidth: true
                            Layout.maximumWidth: implicitWidth
                            visible: page.controlRights

                            text: qsTr("Apply")

                            enabled: prioritySettingsList.changed || settingsPopup.changed

                            onClicked: {
                                //                                if(generatorCard !== null && priTableSyncCheckBox.checked !== generatorCard.enableCopyPriority)
                                //                                    device.send(generatorCard.enableCopyPriorityCommand(priTableSyncCheckBox.checked))
                                if(prioritySettingsList.changed)
                                    device.send(card.commandToSetPriorities(prioritySettingsList.priorities))

                                if(ssmTableCheckBox.checked !== card.enableSSMTable)
                                    device.send(card.enableSSMTableCommand(ssmTableCheckBox.checked))

                                settingsPopup.changed = false
                                prioritySettingsList.changed = false
                            }
                        }
                    }
                }   // ToolBar

            }
        }
    }   // prioritiesSettingsPopupLoader


    Component {
        id: channelTypeSettingsComponent

        Column {
            Controls.ComboBox {
                id: comboBox
                width: parent.width
                focus: true
                value: card.outputChannels[numberOfChannel].type
                model: [
                    SSU.SignalType.Off,
                    SSU.SignalType.KHz2048,
                    SSU.SignalType.E1
                ]
                delegateTextFunc: SSU.signalTypeToString

                readonly property int numberOfChannelsInGroup: Math.round(card.outputChannels.length / 2)
            }

            RowLayout {
                width: parent.width

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: applyToAllChannelsButton
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth
                    text: qsTr("Apply to all")

                    Controls.ToolTip {
                        visible: applyToAllChannelsButton.hovered
                        text: applyToAllChannelsButton.text
                    }

                    onClicked: {
                        for(let i = 0; i < card.outputChannels.length; ++i) {
                            device.send(card.commandToSetTypeOutputChannel(i, comboBox.value))
                        }
                    }
                }

                Button {
                    id: applyToGroupChannelsButton
                    Layout.fillWidth: true
                    Layout.maximumWidth: implicitWidth

                    text: qsTr("To") + " " +
                          (numberOfChannel >= comboBox.numberOfChannelsInGroup ? (comboBox.numberOfChannelsInGroup + 1) + "-" + (card.outputChannels.length)
                                                                               : "1-" + comboBox.numberOfChannelsInGroup) +
                          " " + qsTr("channels")

                    Controls.ToolTip {
                        visible: applyToGroupChannelsButton.hovered
                        text: applyToGroupChannelsButton.text
                    }

                    onClicked: {
                        let start = numberOfChannel >= comboBox.numberOfChannelsInGroup ? comboBox.numberOfChannelsInGroup
                                                                                        : 0
                        let end = numberOfChannel >= comboBox.numberOfChannelsInGroup ? card.outputChannels.length
                                                                                      : comboBox.numberOfChannelsInGroup
                        for(let i = start; i < end; ++i)
                            device.send(card.commandToSetTypeOutputChannel(i, comboBox.value))
                    }
                }
            }

            Button {
                anchors.right: parent.right

                enabled: comboBox.value !== card.outputChannels[numberOfChannel].type
                text: qsTr("Apply")
                onClicked: {
                    device.send(card.commandToSetTypeOutputChannel(numberOfChannel, comboBox.value))
                    controlPopup.close()
                }
            }
        }
    }


    Loader {
        id: outputCardSettingsDialogLoader
        active: false
        sourceComponent: Dialogs.ModalDialog {
            title: qsTr("Settings")
            width: Math.min(Theme.pixelSize * 30, 500); height: Math.min(40 * Theme.pixelSize, 1000)
            onClosing: outputCardSettingsDialogLoader.active = false

            footer: Controls.ToolBar {
                Material.elevation: 0
                visible: page.controlRights
                leftPadding: Theme.indent; rightPadding: Theme.indent

                Row {
                    anchors.right: parent.right;
                    spacing: Theme.middleIndent

                    Button {
                        enabled: ssmControlModeComboBox.value !== card.ssmControlMode ||
                                 ssmComboBox.value !== card.ssm ||
                                 ssmRaiCheckBox.checked !== card.ssmRai ||
                                 enableBypassCheckBox.checked !== card.enableBypass
                        text: qsTr("Apply")

                        onClicked: {
                            if(ssmControlModeComboBox.value !== card.ssmControlMode)
                                device.send(card.commandToSetSsmControlMode(ssmControlModeComboBox.value))
                            if(ssmComboBox.value !== card.ssm)
                                device.send(card.commandToSetSsm(ssmComboBox.value))
                            if(ssmRaiCheckBox.checked !== card.ssmRai)
                                device.send(card.commandToSetSsmRai(ssmRaiCheckBox.checked))
                            if(enableBypassCheckBox.checked !== card.enableBypass)
                                device.send(card.enableBypassCommand(enableBypassCheckBox.checked))
                        }
                    }
                }
            }   // ToolBar

            Column {
                anchors { top: parent.top; left: parent.left; right: parent.right; margins: Theme.indent; }
                enabled: page.controlRights
                //                spacing: Theme.middleIndent
                spacing: 0

                Label {
                    width: parent.width
                    color: Material.accentColor
                    wrapMode: Label.WordWrap
                    text: qsTr("SSM control")
                }

                Controls.ComboBox {
                    id: ssmControlModeComboBox
                    //                    topInset: 0; bottomInset: 0
                    width: parent.width
                    focus: true

                    Binding {
                        target: ssmControlModeComboBox
                        property: "value"
                        value: card.ssmControlMode
                    }

                    model: [
                        Vch003.SsmControlMode.Auto,
                        Vch003.SsmControlMode.Manual
                    ]

                    delegateTextFunc: Vch003.ssmControlModeToString
                }

                Label {
                    width: parent.width
                    color: Material.accentColor
                    wrapMode: Label.WordWrap
                    text: qsTr("SSM")
                }

                RowLayout {
                    width: parent.width

                    Controls.ComboBox {
                        id: ssmComboBox
                        enabled: ssmControlModeComboBox.value === Vch003.SsmControlMode.Manual
                        Layout.fillWidth: true
                        focus: true

                        model: [
                            SSU.SSM.None,
                            SSU.SSM.PRC,
                            SSU.SSM.SSU_T,
                            SSU.SSM.SSU_L,
                            SSU.SSM.SEC,
                            SSU.SSM.DNU
                        ]

                        Binding {
                            target: ssmComboBox
                            property: "value"
                            value: card.ssm
                        }

                        delegateTextFunc: SSU.ssmToString
                    }

                    CheckBox {
                        id: ssmRaiCheckBox
                        text: qsTr("RAI")
                        Binding {
                            target: ssmRaiCheckBox
                            property: "checked"
                            value: card.ssmRai
                        }
                    }
                }   // RowLayout

                CheckBox {
                    id: enableBypassCheckBox
                    width: parent.width
                    text: qsTr("Enable bypass")

                    Binding {
                        target: enableBypassCheckBox
                        property: "checked"
                        value: card.enableBypass
                    }
                }
            }   // Column
        }
    }   // Loader control



}
