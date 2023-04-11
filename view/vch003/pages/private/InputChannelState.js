.import SSU 1.0 as SSU
//.import QtQuick.Controls.Material 2.12 as Material

function stateToolTip(channel) {
    if(channel === null)
        return ""
    switch(channel.type) {
    case SSU.SSU.SignalType.Off:
    case SSU.SSU.SignalType.Auto:
        return SSU.SSU.signalTypeToString(channel.type)
    default: break
    }

    if(channel.ban) {
        return channel.state === 0 ? (qsTr("Ban") + "\n" + qsTr("Disqualification by measurements"))
                                   : (qsTr("Ban") + "\n" + SSU.Vch003.inputSignalStateToStringList(channel.state).join("\n"))
    }
    else if(channel.measurementMode)
        return qsTr("Measurement mode")
    else if(channel.state !== 0)
        return SSU.Vch003.inputSignalStateToStringList(channel.state).join("\n")
    return qsTr("OK")
}

function stateColor(led) {
    //@N Почему-то ругается Property 'color' of object [object Object] is not a function
    if(led === null)
        return "#9E9E9E"                            // Material.Material.color(Material.Material.Grey)
    if(!led.power)
        return "#9E9E9E"                            // Material.Material.Grey
    if(Qt.colorEqual(led.color, "green"))
        return "#8BC34A"                            // Material.Material.LightGreen
    else if(Qt.colorEqual(led.color, "yellow"))
        return "#FF9800"                            // Material.Material.Orange
    return led.color
}
