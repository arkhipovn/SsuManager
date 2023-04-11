#include "ptp.h"

namespace ssu {
    Ptp::Ptp(QObject *parent) : QObject(parent),
        valid_(false),

        ptpMode_(PtpMode::Master),
        txMode_(TxMode::Multicast),
        wayMode_(WayMode::OneWay),
        syncMode_(SyncMode::OneStep),
        delayMechanism_(DelayMechanism::E2E),
        domainNumber_(0),
        protocol_(Protocol::Ipv4),

        announceTimeout_(0),
        syncInterval_(0),
        delayRequest_(0),
        pDelayRequest_(0),

        priority1_(0),
        priority2_(0),
        clockClass_(0)
    {

    }

    Ptp::PtpMode Ptp::ptpMode() const
    {
        return ptpMode_;
    }

    void Ptp::setPtpMode(PtpMode ptpMode)
    {
        if(ptpMode_ != ptpMode) {
            ptpMode_ = ptpMode;
            emit ptpModeChanged();
        }
    }

    Ptp::TxMode Ptp::txMode() const
    {
        return txMode_;
    }

    void Ptp::setTxMode(TxMode txMode)
    {
        if(txMode_ != txMode) {
            txMode_ = txMode;
            emit txModeChanged();
        }
    }

    Ptp::WayMode Ptp::wayMode() const
    {
        return wayMode_;
    }

    void Ptp::setWayMode(WayMode wayMode)
    {
        if(wayMode_ != wayMode) {
            wayMode_ = wayMode;
            emit wayModeChanged();
        }
    }

    Ptp::SyncMode Ptp::syncMode() const
    {
        return syncMode_;
    }

    void Ptp::setSyncMode(SyncMode syncMode)
    {
        if(syncMode_ != syncMode) {
            syncMode_ = syncMode;
            emit syncModeChanged();
        }
    }

    Ptp::DelayMechanism Ptp::delayMechanism() const
    {
        return delayMechanism_;
    }

    void Ptp::setDelayMechanism(DelayMechanism delayMechanism)
    {
        if(delayMechanism_ != delayMechanism) {
            delayMechanism_ = delayMechanism;
            emit delayMechanismChanged();
        }
    }

    int Ptp::domainNumber() const
    {
        return domainNumber_;
    }

    void Ptp::setDomainNumber(int domainNumber)
    {
        if(domainNumber_ != domainNumber) {
            domainNumber_ = domainNumber;
            emit domainNumberChanged();
        }
    }

    Ptp::Protocol Ptp::protocol() const
    {
        return protocol_;
    }

    void Ptp::setProtocol(Protocol protocol)
    {
        if(protocol_ != protocol) {
            protocol_ = protocol;
            emit protocolChanged();
        }
    }

    int Ptp::announceTimeout() const
    {
        return announceTimeout_;
    }

    void Ptp::setAnnounceTimeout(int announceTimeout)
    {
        if(announceTimeout_ != announceTimeout) {
            announceTimeout_ = announceTimeout;
            emit announceTimeoutChanged();
        }
    }

    int Ptp::syncInterval() const
    {
        return syncInterval_;
    }

    void Ptp::setSyncInterval(int syncInterval)
    {
        if(syncInterval_ != syncInterval) {
            syncInterval_ = syncInterval;
            emit syncIntervalChanged();
        }
    }

    int Ptp::delayRequest() const
    {
        return delayRequest_;
    }

    void Ptp::setDelayRequest(int delayRequest)
    {
        if(delayRequest_ != delayRequest) {
            delayRequest_ = delayRequest;
            emit delayRequestChanged();
        }
    }

    int Ptp::pDelayRequest() const
    {
        return pDelayRequest_;
    }

    void Ptp::setPDelayRequest(int pDelayRequest)
    {
        if(pDelayRequest_ != pDelayRequest) {
            pDelayRequest_ = pDelayRequest;
            emit pDelayRequestChanged();
        }
    }

    int Ptp::priority1() const
    {
        return priority1_;
    }

    void Ptp::setPriority1(int priority1)
    {
        if(priority1_ != priority1) {
            priority1_ = priority1;
            emit priority1Changed();
        }
    }

    int Ptp::priority2() const
    {
        return priority2_;
    }

    void Ptp::setPriority2(int priority2)
    {
        if(priority2_ != priority2) {
            priority2_ = priority2;
            emit priority2Changed();
        }
    }

    int Ptp::clockClass() const
    {
        return clockClass_;
    }

    void Ptp::setClockClass(int clockClass)
    {
        if(clockClass_ != clockClass) {
            clockClass_ = clockClass;
            emit clockClassChanged();
        }
    }

    bool Ptp::isValid() const
    {
        return valid_;
    }

    void Ptp::setValid(bool valid)
    {
        if(valid_ != valid) {
            valid_ = valid;
            emit validChanged();
        }
    }

    QString Ptp::clockId() const
    {
        return clockId_;
    }

    void Ptp::setClockId(const QString &clockId)
    {
        if(clockId_ != clockId) {
            clockId_ = clockId;
            emit clockIdChanged();
        }
    }

    QString Ptp::remoteMasterAddress() const
    {
        return remoteMasterAddress_;
    }

    void Ptp::setRemoteMasterAddress(const QString &remoteMasterAddress)
    {
        if(remoteMasterAddress_ != remoteMasterAddress) {
            remoteMasterAddress_ = remoteMasterAddress;
            emit remoteMasterAddressChanged();
        }
    }
}
