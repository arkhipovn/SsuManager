#ifndef PTP_H
#define PTP_H

#include <QObject>
#include "ssu.h"

namespace ssu {
    class Ptp : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)

        Q_PROPERTY(PtpMode ptpMode READ ptpMode NOTIFY ptpModeChanged)
        Q_PROPERTY(TxMode txMode READ txMode NOTIFY txModeChanged)
        Q_PROPERTY(WayMode wayMode READ wayMode NOTIFY wayModeChanged)
        Q_PROPERTY(SyncMode syncMode READ syncMode NOTIFY syncModeChanged)
        Q_PROPERTY(DelayMechanism delayMechanism READ delayMechanism NOTIFY delayMechanismChanged)
        Q_PROPERTY(int domainNumber READ domainNumber NOTIFY domainNumberChanged)
        Q_PROPERTY(Protocol protocol READ protocol NOTIFY protocolChanged)

        Q_PROPERTY(int announceTimeout READ announceTimeout NOTIFY announceTimeoutChanged)
        Q_PROPERTY(int syncInterval READ syncInterval NOTIFY syncIntervalChanged)
        Q_PROPERTY(int delayRequest READ delayRequest NOTIFY delayRequestChanged)
        Q_PROPERTY(int pDelayRequest READ pDelayRequest NOTIFY pDelayRequestChanged)
        Q_PROPERTY(int priority1 READ priority1 NOTIFY priority1Changed)
        Q_PROPERTY(int priority2 READ priority2 NOTIFY priority2Changed)
        Q_PROPERTY(int clockClass READ clockClass NOTIFY clockClassChanged)

        Q_PROPERTY(QString clockId READ clockId NOTIFY clockIdChanged)
        Q_PROPERTY(QString remoteMasterAddress READ remoteMasterAddress NOTIFY remoteMasterAddressChanged)

    public:
        explicit Ptp(QObject *parent = nullptr);

        enum class PtpMode {
            Invalid,
            Master,
            Slave
        };

        Q_ENUMS(PtpMode)

        enum class TxMode {
            Invalid,
            Unicast,
            Multicast,
            Mixed
        };

        Q_ENUMS(TxMode)

        enum class WayMode {
            Invalid,
            OneWay,
            TwoWay
        };

        Q_ENUMS(WayMode)

        enum class SyncMode {
            Invalid,
            OneStep,
            TwoStep
        };

        Q_ENUMS(SyncMode)

        enum class DelayMechanism {
            Invalid,
            E2E,
            P2P
        };

        Q_ENUMS(DelayMechanism)

        enum class Protocol {
            Invalid,
            L2MCast1,
            L2MCast2,
            Ipv4,
            Ipv6
        };

        Q_ENUMS(Protocol)

        PtpMode ptpMode() const;
        void setPtpMode(PtpMode ptpMode);

        TxMode txMode() const;
        void setTxMode(TxMode txMode);

        WayMode wayMode() const;
        void setWayMode(WayMode wayMode);

        SyncMode syncMode() const;
        void setSyncMode(SyncMode syncMode);

        DelayMechanism delayMechanism() const;
        void setDelayMechanism(DelayMechanism delayMechanism);

        int domainNumber() const;
        void setDomainNumber(int domainNumber);

        Protocol protocol() const;
        void setProtocol(Protocol protocol);

        int announceTimeout() const;
        void setAnnounceTimeout(int announceTimeout);

        int syncInterval() const;
        void setSyncInterval(int syncInterval);

        int delayRequest() const;
        void setDelayRequest(int delayRequest);

        int pDelayRequest() const;
        void setPDelayRequest(int pDelayRequest);

        int priority1() const;
        void setPriority1(int priority1);

        int priority2() const;
        void setPriority2(int priority2);

        int clockClass() const;
        void setClockClass(int clockClass);

        bool isValid() const;
        void setValid(bool valid);

        QString clockId() const;
        void setClockId(const QString &clockId);

        QString remoteMasterAddress() const;
        void setRemoteMasterAddress(const QString &remoteMasterAddress);

    signals:
        void validChanged();

        void ptpModeChanged();
        void txModeChanged();
        void wayModeChanged();
        void syncModeChanged();
        void delayMechanismChanged();
        void domainNumberChanged();
        void protocolChanged();

        void announceTimeoutChanged();
        void syncIntervalChanged();
        void delayRequestChanged();
        void pDelayRequestChanged();

        void priority1Changed();
        void priority2Changed();
        void clockClassChanged();

        void clockIdChanged();
        void remoteMasterAddressChanged();

    private:
        bool valid_;

        QString clockId_;   // Идентификатор часов PTP

        // Configuration
        PtpMode ptpMode_;
        TxMode txMode_;
        WayMode wayMode_;
        SyncMode syncMode_;
        DelayMechanism delayMechanism_;
        int domainNumber_;
        Protocol protocol_;

        // Intervals
        int announceTimeout_;
        int syncInterval_;
        int delayRequest_;
        int pDelayRequest_;

        // Announce
        int priority1_;
        int priority2_;
        int clockClass_;

        QString remoteMasterAddress_;   // IPv4 адрес удаленного PTP Master
    };
}

#endif // PTP_H
