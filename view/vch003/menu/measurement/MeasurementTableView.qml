import QtQuick 2.12
import Controls 1.0 as Controls

import Models.Meas 1.0
import QCC.Filters 2.0

import Theme 1.0

Controls.TableView {
    id: tableView
    width: parent.width; height: parent.height

    columnWidthProvider: function (column) {
        switch(column) {
        case MeasLogModel.Time:
            return Theme.pixelSize * 13
        case MeasLogModel.Channel:
            return Theme.pixelSize * 15
        default: break
        }
        return Theme.pixelSize * 9
    }

    property alias sourceModel: sortFilterProxyTableModel.sourceModel

    model: SortFilterProxyTableModel {
        id: sortFilterProxyTableModel
        sortOrder: tableView.sortIndicatorOrder
        sortCaseSensitivity: Qt.CaseInsensitive
        sortColumn: tableView.sortIndicatorColumn
    }
}
