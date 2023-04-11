import QtQuick 2.12
import QtQuick.Layouts 1.12
import Theme 1.0

Item {
    implicitHeight: Theme.rowHeight

    default property alias content: rowLayout.data
    property alias spacing: rowLayout.spacing

    RowLayout {
        id: rowLayout
        anchors { left: parent.left; right: parent.right; margins: Theme.smallIndent; }
        height: parent.height
        spacing: Theme.smallIndent
    }
}
