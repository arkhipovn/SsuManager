import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import QCC.DataView.TableView 1.0 as DV

import Controls 1.0 as Controls

DV.TableView {
    id: tableView

    elevation: 3

    dateFormat: programSettings.dateTimeFormat

    verticalHeader.visible: false

    selectionBehavior: DV.SelectionModel.SelectRows
    sortIndicatorVisible: true
    selectedColor: Material.accentColor

    wrapMode: Text.NoWrap
    elide: Qt.ElideRight

    toolTip: Controls.ToolTip { delay: 1000; }
}
