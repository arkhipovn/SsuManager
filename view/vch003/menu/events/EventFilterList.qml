import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import Theme 1.0
import Models.Log 1.0

import SSU 1.0
import Controls 1.0 as Controls

import Dialogs 1.0 as Dialogs
import QCC.Controls 1.0 as QCC

Popup {
    modal: true

    leftMargin: Theme.indent; rightMargin: Theme.indent
    topMargin: Theme.indent; bottomMargin: Theme.indent

    topPadding: Theme.indent; bottomPadding: Theme.indent
    leftPadding: Theme.indent; rightPadding: Theme.indent

    signal filtersCleared()

    property var device: null
    property var filters: null
    property var sourceModel: null

    default property alias header: headerColumn.data
    property alias footer: footerColumn.data

    clip: true

    function setStatusFilter(status) {
        if(sourceModel == null || filters == null)
            return
        filters.clear()
        let statusId = 2    //@N убрать магическое число..
        let statusListElement = filterTypeListRepeater.model.get(statusId)
        filters.append(statusListElement.column, sourceModel.roleKey(statusListElement.role), status, false, statusId)  //@N sourceModel.roleKey(role)..
    }

    contentItem: ColumnLayout {
        spacing: 0

        Column {
            id: headerColumn
            Layout.fillWidth: true;
        }

        ListView {
            visible: filters != null
            Layout.fillWidth: true; Layout.fillHeight: true
            model: filters

            boundsBehavior: ListView.StopAtBounds
            flickableDirection: ListView.VerticalFlick
            headerPositioning: ListView.OverlayHeader

            interactive: contentHeight > height

            header: Column {
                width: parent.width

                RowLayout {
                    width: parent.width

                    ComboBox {
                        Layout.fillWidth: true
                        model: [ qsTr("And"), qsTr("Or") ]
                        currentIndex: filters == null ? 0
                                                      : filters.typeCondition
                        onCurrentIndexChanged: {
                            if(filters != null) {
                                filters.typeCondition = currentIndex
                            }
                        }
                    }

                    Button {
                        text: qsTr("Clear filters")
                        onClicked: {
                            if(filters != null)
                                filters.clear()
                            filtersCleared()
                        }
                    }
                }

                Button {
                    width: parent.width
                    text: qsTr("Add filter")
                    onClicked: addFilterPopup.open()
                }
            }

            delegate: MouseArea {
                id: mouseArea
                width: ListView.view.width; height: rowLayout.height
                hoverEnabled: true

                RowLayout {
                    id: rowLayout
                    width: parent.width;

                    Loader {
                        Layout.fillWidth: true;
                        Layout.alignment: Qt.AlignVCenter
                        property var filterData: model
                        sourceComponent: filterTypeListRepeater.model.get(filterId).component
                    }

                    QCC.SymbolButton {
                        Layout.alignment: Qt.AlignVCenter
                        topInset: 0; bottomInset: 0
                        visible: mouseArea.containsMouse
                        symbol: "\uf00d"
                        onClicked: filters.removeAt(index)
                    }
                }
            }
        }

        Column {
            id: footerColumn
            Layout.fillWidth: true;
        }
    }

    Component {
        id: slotFilterComponent

        EventFilter {
            id: filter
            model: {
                var list = [""];
                if(device == null)
                    return list
                for(var i = 0; i < device.options.kNumberOfSlots; ++i) {
                    list.push("A" + i.toString().padStart(2, "0"))
                }
                return list
            }

            displayText: currentIndex == 0 ? qsTr("Slot filter") : currentText
            delegateTextFunc: function(value) { return value === "" ? qsTr("No filter") : value }
        }
    }

    Component {
        id: cardTypeFilterComponent

        EventFilter {
            model: {
                var list = [""];
                if(device == null)
                    return list
                list.push.apply(list, device.panel.cardTypes)
                return list
            }

            displayText: currentIndex == 0  ? qsTr("Card filter") : SSU.cardTypeToString(currentText)
            delegateTextFunc: function(value) { return value === "" ? qsTr("No filter") : SSU.cardTypeToString(value) }
        }
    }

    Component {
        id: eventStatusFilterComponent

        EventFilter {
            model: ["", SSU.Critical, SSU.Major, SSU.Minor, SSU.Warning]

            displayText: currentIndex == 0 ? qsTr("Status filter") : SSU.eventStatusToString(currentText)
            delegateTextFunc: function(value) { return value === "" ? qsTr("No filter") : SSU.eventStatusToString(value) }
        }
    }

    //    Component {
    //        id: slotFilterComponent
    //        EventFilter {
    //            width: parent.width
    //            role: "channel"
    //            model: {
    //                var list = [""];
    //                if(device == null)
    //                    return list
    //                device.options
    //            }

    //            displayText: currentIndex == 0 ? qsTr("Channel filter") : SSU.eventStatusToString(currentText)
    //            delegateTextFunc: function(value) { return value === "" ? qsTr("No filter") : SSU.eventStatusToString(value) }
    //    }

    Dialogs.SettingsPopup {
        id: addFilterPopup
        y: Theme.indent
        width: parent.width

        title: qsTr("Adding a filter")
        leftPadding: 0; rightPadding: 0

        contentItem: Column {
            Repeater {
                id: filterTypeListRepeater
                model: ListModel {
                    Component.onCompleted: {
                        append({
                                   "name": qsTr("Slot filter"),
                                   "column": EventLogModel.SlotColumn,
                                   "role": "display",
                                   "component": slotFilterComponent
                               })
                        append({
                                   "name": qsTr("Card filter"),
                                   "column": 0,
                                   "role": "cardType",
                                   "component": cardTypeFilterComponent
                               })
                        append({
                                   "name": qsTr("Status filter"),
                                   "column": 0,
                                   "role": "eventStatus",
                                   "component": eventStatusFilterComponent
                               })
                    }
                }

                delegate: Controls.IconItemDelegate {
                    leftPadding: Theme.indent; rightPadding: Theme.indent
                    text: name
                    symbol: "\uf067"
                    symbolColor: Material.accentColor
                    width: parent.width
                    onClicked: {
                        filters.append(column, sourceModel.roleKey(role), "", false, index)  //@N sourceModel.roleKey(role)..
                        addFilterPopup.close()
                    }
                }
            }
        }
    }


}

