#ifndef PORTSTATISTICS_H
#define PORTSTATISTICS_H

#include <QObject>

//@N Мб сделать как QObject?

namespace ssu {
    struct PortStatistics {
    private:
        Q_GADGET

        Q_PROPERTY(int rxL2MIIcounter MEMBER rxL2MIIcounter)
        Q_PROPERTY(int txL2MIIcounter MEMBER txL2MIIcounter)

        Q_PROPERTY(int rxL2Good MEMBER rxL2Good)
        Q_PROPERTY(int rxL2Fail MEMBER rxL2Fail)

        Q_PROPERTY(int txL2Good MEMBER txL2Good)
        Q_PROPERTY(int txL2Fail MEMBER txL2Fail)

        Q_PROPERTY(int rxL2Bandwidth MEMBER rxL2Bandwidth)
        Q_PROPERTY(int txL2Bandwidth MEMBER txL2Bandwidth)

        Q_PROPERTY(int numberOfArpRxL2 MEMBER numberOfArpRxL2)
        Q_PROPERTY(int numberOfArpTxL2 MEMBER numberOfArpTxL2)

        Q_PROPERTY(int numberOfIcmpRxL2 MEMBER numberOfIcmpRxL2)
        Q_PROPERTY(int numberOfIcmpTxL2 MEMBER numberOfIcmpTxL2)

        Q_PROPERTY(int numberOfNtpRxL2 MEMBER numberOfNtpRxL2)
        Q_PROPERTY(int numberOfNtpTxL2 MEMBER numberOfNtpTxL2)

        Q_PROPERTY(int numberOfPtpRxL2 MEMBER numberOfPtpRxL2)
        Q_PROPERTY(int numberOfPtpTxL2 MEMBER numberOfPtpTxL2)


        Q_PROPERTY(int rxFromEthAnnounce MEMBER rxFromEthAnnounce)
        Q_PROPERTY(int rxFromEthSync MEMBER rxFromEthSync)
        Q_PROPERTY(int rxFromEthFollowUp MEMBER rxFromEthFollowUp)
        Q_PROPERTY(int rxFromEthDelayReq MEMBER rxFromEthDelayReq)
        Q_PROPERTY(int rxFromEthPDelayReq MEMBER rxFromEthPDelayReq)
        Q_PROPERTY(int rxFromEthTLVsReq MEMBER rxFromEthTLVsReq)
        Q_PROPERTY(int rxFromEthDelayResp MEMBER rxFromEthDelayResp)
        Q_PROPERTY(int rxFromEthPDelayResp MEMBER rxFromEthPDelayResp)
        Q_PROPERTY(int rxFromEthPDelayRespFollowUp MEMBER rxFromEthPDelayRespFollowUp)
        Q_PROPERTY(int rxFromEthTLVsResp MEMBER rxFromEthTLVsResp)

        Q_PROPERTY(int txFromEthAnnounce MEMBER txFromEthAnnounce)
        Q_PROPERTY(int txFromEthSync MEMBER txFromEthSync)
        Q_PROPERTY(int txFromEthFollowUp MEMBER txFromEthFollowUp)
        Q_PROPERTY(int txFromEthDelayReq MEMBER txFromEthDelayReq)
        Q_PROPERTY(int txFromEthPDelayReq MEMBER txFromEthPDelayReq)
        Q_PROPERTY(int txFromEthTLVsReq MEMBER txFromEthTLVsReq)
        Q_PROPERTY(int txFromEthDelayResp MEMBER txFromEthDelayResp)
        Q_PROPERTY(int txFromEthPDelayResp MEMBER txFromEthPDelayResp)
        Q_PROPERTY(int txFromEthPDelayRespFollowUp MEMBER txFromEthPDelayRespFollowUp)
        Q_PROPERTY(int txFromEthTLVsResp MEMBER txFromEthTLVsResp)

    public:
        int rxL2MIIcounter = 0;     // Счетчик принимаемых L2 пакетов из PHY
        int txL2MIIcounter = 0;     // Счетчик передаваемых L2 пакетов из MII/GMII -> PHY интерфейса:

        int rxL2Good = 0;           // Счетчик принимаемых L2 пакетов, у которых сошлась контрольная сумма (CRC32)
        int rxL2Fail = 0;           // Счетчик принимаемых L2 пакетов, у которых не сошлась контрольная сумма (CRC32)

        int txL2Good = 0;           // Счетчик передаваемых L2 пакетов, у которых сошлась контрольная сумма (CRC32):
        int txL2Fail = 0;           // Счетчик передаваемых L2 пакетов, у которых не сошлась контрольная сумма (CRC32)

        int rxL2Bandwidth = 0;      // Пропускная способность (бит/c) принимаемых из сети L2 пакетов (максимальная 999000000 бит/c)
        int txL2Bandwidth = 0;      // Пропускная способность (бит/c) передаваемых в сеть L2 пакетов (максимальная 999000000 бит/c)

        int numberOfArpRxL2 = 0;    // Общий счетчик принимаемых L2 для ARP
        int numberOfArpTxL2 = 0;    // Общий счетчик передаваемых L2 для ARP

        int numberOfIcmpRxL2 = 0;   // Общий счетчик принимаемых L2 для ICMP
        int numberOfIcmpTxL2 = 0;   // Общий счетчик передаваемых L2 для ICMP

        int numberOfNtpRxL2 = 0;    // Общий счетчик принимаемых L2 для NTP
        int numberOfNtpTxL2 = 0;    // Общий счетчик передаваемых L2 для NTP

        int numberOfPtpRxL2 = 0;    // Общий счетчик принимаемых L2 для PTP
        int numberOfPtpTxL2 = 0;    // Общий счетчик передаваемых L2 для PTP

        int rxFromEthAnnounce = 0;
        int rxFromEthSync = 0;
        int rxFromEthFollowUp = 0;
        int rxFromEthDelayReq = 0;
        int rxFromEthPDelayReq = 0;
        int rxFromEthTLVsReq = 0;
        int rxFromEthDelayResp = 0;
        int rxFromEthPDelayResp = 0;
        int rxFromEthPDelayRespFollowUp = 0;
        int rxFromEthTLVsResp = 0;

        int txFromEthAnnounce = 0;
        int txFromEthSync = 0;
        int txFromEthFollowUp = 0;
        int txFromEthDelayReq = 0;
        int txFromEthPDelayReq = 0;
        int txFromEthTLVsReq = 0;
        int txFromEthDelayResp = 0;
        int txFromEthPDelayResp = 0;
        int txFromEthPDelayRespFollowUp = 0;
        int txFromEthTLVsResp = 0;
    };

}

Q_DECLARE_METATYPE(ssu::PortStatistics)

#endif // PORTSTATISTICS_H
