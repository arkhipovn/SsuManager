import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import Theme 1.0
import QCC.Controls 1.0 as QCC

import QtQuick.Layouts 1.12

Column {
    id: portStatistics
    property var model: null

    function updatePortStatistics() {
        device.send(card.updatePortStatisticsCommand(portModel))
    }

    Component.onCompleted: updatePortStatistics()

    Row {
        QCC.SymbolButton {
            text: qsTr("Update")
            symbol: "\uf021"
            onClicked: {
                portStatistics.updatePortStatistics()
                timer.restart()
            }

            Timer {
                id: timer
                repeat: true
                interval: 5000  //@N задать время..
                running: true
                onTriggered: portStatistics.updatePortStatistics()
            }
        }

        QCC.SymbolButton {
            text: qsTr("Clear")
            symbol: "\uf1f8"
            color: Material.color(Material.Red)
            onClicked: {
                device.send(card.clearPortStatisticsCommand(portModel))
                //                portStatistics.updatePortStatistics()
                timer.restart()
            }
        }
    }

    GridLayout {
        width: parent.width
        columns: 6  // 6

        rowSpacing: Theme.middleIndent
        columnSpacing: Theme.indent

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Counter of received L2 packets from PHY") // qsTr("rxL2MIIcounter")    // Счетчик принимаемых L2 пакетов из PHY
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxL2MIIcounter
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Counter of transmitted L2 packets in PHY") //qsTr("txL2MIIcounter")    // Счетчик передаваемых L2 пакетов из MII/GMII -> PHY интерфейса:
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txL2MIIcounter
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Count of received L2 packets that matched the checksum (CRC32)")//qsTr("rxL2Good")     // Счетчик принимаемых L2 пакетов, у которых сошлась контрольная сумма (CRC32)
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxL2Good
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Count of received L2 packets that did not match the checksum (CRC32)")  // Счетчик принимаемых L2 пакетов, у которых не сошлась контрольная сумма (CRC32)
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxL2Fail
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Count of transmitted L2 packets that matched the checksum (CRC32)")// qsTr("txL2Good")  // Счетчик передаваемых L2 пакетов, у которых сошлась контрольная сумма (CRC32):
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txL2Good
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Count of transmitted L2 packets for which the checksum did not agree (CRC32)")  //qsTr("txL2Fail")  // Счетчик передаваемых L2 пакетов, у которых не сошлась контрольная сумма (CRC32)
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txL2Fail
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Bandwidth (bps) of packets received from the L2 network")   // qsTr("rxL2Bandwidth")     // Пропускная способность (бит/c) принимаемых из сети L2 пакетов (максимальная 999000000 бит/c)
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxL2Bandwidth
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Bandwidth (bps) of packets transmitted to the L2 network") // qsTr("txL2Bandwidth")     // Пропускная способность (бит/c) передаваемых в сеть L2 пакетов (максимальная 999000000 бит/c)
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txL2Bandwidth
        }

        // _______________


        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of received L2 for ARP")    // qsTr("numberOfArpRxL2")       // Общий счетчик принимаемых L2 для ARP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfArpRxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of transmitted L2 for ARP")  // qsTr("numberOfArpTxL2")        // Общий счетчик передаваемых L2 для ARP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfArpTxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of received L2 for ICMP")    //qsTr("numberOfIcmpRxL2")        // Общий счетчик принимаемых L2 для ICMP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfIcmpRxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of transmitted L2 for ICMP")  //qsTr("numberOfIcmpTxL2")      // Общий счетчик передаваемых L2 для ICMP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfIcmpTxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of received L2 for NTP")    // qsTr("numberOfNtpRxL2")       // Общий счетчик принимаемых L2 для NTP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfNtpRxL2
        }


        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of transmitted L2 for NTP")  // qsTr("numberOfNtpTxL2")        // Общий счетчик передаваемых L2 для NTP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfNtpTxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of received L2 for PTP")    // qsTr("numberOfPtpRxL2")        // Общий счетчик принимаемых L2 для PTP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfPtpRxL2
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Number of transmitted L2 for PTP")  // qsTr("numberOfPtpTxL2")        // Общий счетчик передаваемых L2 для PTP
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.numberOfPtpTxL2
        }


        //_______________



        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth announce")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthAnnounce
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth sync")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthSync
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth follow up")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthFollowUp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth Delay req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthDelayReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth PDelay req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthPDelayReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth TLVs req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthTLVsReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth Delay resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthDelayResp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth PDelay resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthPDelayResp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth PDelay resp follow up")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthPDelayRespFollowUp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Rx from eth TLVs resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.rxFromEthTLVsResp
        }









        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth announce")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthAnnounce
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth sync")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthSync
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth follow up")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthFollowUp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth Delay req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthDelayReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth PDelay req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthPDelayReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth TLVs req")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthTLVsReq
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth Delay resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthDelayResp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth PDelay resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthPDelayResp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth PDelay resp follow up")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthPDelayRespFollowUp
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            text: qsTr("Tx from eth TLVs resp")
            // font.bold: true
        }

        Label {
            color: Material.accentColor
            text: model == null ? "-"
                                : model.txFromEthTLVsResp
        }

    }


}
