import QtQuick 2.12
import QtQuick.Controls 2.12

Flickable {
    width: menuColumn.width

    clip: true
    boundsBehavior: Flickable.StopAtBounds
    interactive: contentHeight > height
    flickableDirection: Flickable.VerticalFlick

    contentHeight: menuColumn.height
    default property alias contentMenu: menuColumn.data

    Column {
        id: menuColumn
    }
}
