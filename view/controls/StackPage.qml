import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0

Page {
    topPadding: Theme.middleIndent; bottomPadding: 0
    leftPadding: Theme.indent; rightPadding: Theme.indent
    signal comeback()

    //    Keys.onPressed: {
    //        if(event.key === Qt.Key_Backspace) {
    //            console.log("Key_Backspace");
    //        }
    //    }
}
