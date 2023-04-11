.import Models.Port 1.0 as Port

function minToDateString(min) {
    return Math.trunc(min / 60).toString().padStart(2, "0") + ":" + (min % 60).toString().padStart(2, "0")
}

function ptpClockStatusToString(value) {
    switch(value) {
    case Port.EthernetPort.Lock:        return qsTr("Lock")
    case Port.EthernetPort.NotLock:     return qsTr("Not Lock")
    default:
        return "-"
    }
}

function ppsSourceToString(value) {
    switch(value) {
    case Port.EthernetPort.MasterInternal:  return qsTr("Master: Internal osc")
    case Port.EthernetPort.MasterGnss:      return qsTr("Master: GNSS")
    case Port.EthernetPort.MasterSlave:     return qsTr("Master: Slave")
    case Port.EthernetPort.Slave:           return qsTr("Slave")
    default:
        return "-"
    }
}

function portSpeedToString(value) {
    switch(value) {
    case Port.EthernetPort.Auto:       return qsTr("Auto")
    case Port.EthernetPort.NoCapture:  return qsTr("No capture")
    case Port.EthernetPort.M1000:      return qsTr("1000")
    case Port.EthernetPort.M100:       return qsTr("100")
    case Port.EthernetPort.M10:        return qsTr("10")
    default:
        return "-"
    }
}

function ptpModeToString(value)
{
    switch (value) {
    case Port.Ptp.Master:  return qsTr("Master");
    case Port.Ptp.Slave:   return qsTr("Slave");
    default: return "-";
    }
}

function txModeToString(value)
{
    switch (value) {
    case Port.Ptp.Unicast:      return qsTr("Unicast");
    case Port.Ptp.Multicast:    return qsTr("Multicast");
    case Port.Ptp.Mixed:        return qsTr("Mixed");
    default: return "-";
    }
}

function wayModeToString(value)
{
    switch (value) {
    case Port.Ptp.OneWay:  return qsTr("One-Way");
    case Port.Ptp.TwoWay:  return qsTr("Two-Way");
    default: return "-";
    }
}

function syncModeToString(value)
{
    switch (value) {
    case Port.Ptp.OneStep:    return qsTr("One-Step");
    case Port.Ptp.TwoStep:    return qsTr("Two-Step");
    default: return "-";
    }
}

function delayMechanismToString(value)
{
    switch (value) {
    case Port.Ptp.E2E:  return qsTr("E2E");
    case Port.Ptp.P2P:  return qsTr("P2P");
    default: return "-";
    }
}

function protocolToString(value)
{
    switch (value) {
    case Port.Ptp.Ipv4:       return qsTr("Ipv4");
    case Port.Ptp.Ipv6:       return qsTr("Ipv6");
    case Port.Ptp.L2MCast1:   return qsTr("L2 MCast 1");
    case Port.Ptp.L2MCast2:   return qsTr("L2 MCast 2");
    default: return "-";
    }
}
