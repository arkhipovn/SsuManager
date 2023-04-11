import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import Theme 1.0

Pane {
    id: row
    implicitHeight: Theme.rowHeight + topPadding + bottomPadding

    default property alias content: rowLayout.data
    property alias backgroundContent: backgroundRec.data

    property alias colorBehavior: behavior.enabled
    property alias seperatorActive: separator.visible
    property color color: "#FAFAFA"// "#E7EBF4"
    property alias backgroundColor: backgroundRec.color

    property bool highlighted: false

    spacing: Theme.middleIndent

    padding: 0
    topPadding: 0; bottomPadding: 0
    leftPadding: 0; rightPadding: 0

    background: Rectangle {
        id: backgroundRec
        color: highlighted || row.hovered ? row.color : row.Material.backgroundColor

        Behavior on color {
            id: behavior
            enabled: true
            ColorAnimation { duration: 200; }
        }

        Separator {
            id: separator
            z: 2
            visible: false
            y: parent.height - height;
            orientation: Qt.Horizontal;
            color: highlighted || row.hovered ? Material.accentColor : Material.dropShadowColor
        }
    }

    contentItem: RowLayout {
        id: rowLayout
        width: parent.width; height: parent.height
        spacing: row.spacing
        clip: true
    }
}


