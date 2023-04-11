import QtQuick 2.12
import SSU 1.0

import "vch003/" as Vch003

Loader {
    id: pageLoader
    property var device: null

    sourceComponent: {
        if(device == null) return null
        switch(device.type) {
        case SSU.Vch003: return vch003Form
        default: return null
        }
    }

    Component {
        id: vch003Form
        Vch003.DevicePage {
            device: pageLoader.device
        }
    }
}
