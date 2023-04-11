import Controls 1.0 as Controls
import QtQuick.Controls.Material 2.12
import SSU 1.0

Controls.Button {
    property int state: 0
    property string title: ""

    backgroundColor: {
        switch(state) {
        case SSU.PllState.Warmup:        return Material.color(Material.Orange)
        case SSU.PllState.Freerun:
        case SSU.PllState.Holdover:      return Material.color(Material.Orange)
        case SSU.PllState.Adjustment:    return Material.color(Material.Teal)
        case SSU.PllState.Tracking:      return Material.color(Material.LightGreen)
        case SSU.PllState.Error:         return Material.color(Material.Red)
        default: return Material.color(Material.LightGreen)
        }
    }

    clickable: false
    symbol: SSU.pllStateToString(state) + (title.length > 0 ? "\r\n" + title : "")
    symbolLabel.horizontalAlignment: Qt.AlignHCenter
}
