import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Theme 1.0

Pane {
    id: row

    default property alias content: rowLayout.data

    implicitHeight: Theme.rowHeight + topPadding + bottomPadding
    z: 2;
    topInset: 0; bottomInset: 0
    padding: 0
    spacing: 0

    contentItem: RowLayout {
        id: rowLayout
        width: parent.width; height: parent.height
        spacing: row.spacing
    }
}
