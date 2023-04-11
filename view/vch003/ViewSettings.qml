import QtQuick 2.0
import QtQuick.Controls.Material 2.12
import SSU 1.0

QtObject {
    readonly property color panelColor: "#E6E6E6"

    readonly property color panelTitleColor: "#2F2D2D"// "#444444"
    readonly property string panelFontFamily:  "Tahoma"  //  "Tahoma"   "HoloLens MDL2 Assets"  "Arial"

    function itemSpacing(widthCoef, heightCoef) {
        return Math.min(widthCoef * 53, heightCoef * 35)
    }

    function ledDiameter(widthCoef, heightCoef) {
        return Math.min(widthCoef * 20, heightCoef * 20)
    }

    function pixelSize(widthCoef, heightCoef) {
        return Math.min(widthCoef * 20, heightCoef * 20)
        //        return Math.min(widthCoef * 18, heightCoef * 18)
    }

    function bigPixelSize(widthCoef, heightCoef) {
        return Math.min(widthCoef * 24, heightCoef * 24)
    }

    readonly property real heightPairLed: 155       // Высота для пары светодиодов на плате


    function cardTitle(type) {
        switch(type) {
        case SSU.Input:         return qsTr("Input")
        case SSU.Generator:     return qsTr("Clk")
        case SSU.Output:        return qsTr("Output")
        case SSU.Monitoring:    return qsTr("BM")
        case SSU.Power:         return qsTr("Power")
        case SSU.Gnss:          return qsTr("GNSS")
        case SSU.Distribution:  return qsTr("Distrib")
        default: break
        }
        return qsTr("N/A")
    }
}
