#include "ethernetport.h"
#include "vch003/configuration/vch003options.h"

namespace ssu {
    EthernetPort::EthernetPort(QObject *parent) : QObject(parent),
        ppsSource_(PpsSource::MasterGnss),
        //        ptpClockStatus_(PtpClockStatus::Lock),
        speed_(Speed::NoCapture),
        mtu_(0),
        swap_(false),
        loopback_(false),
        syncEMode_(false),
        qlMode_(false),
        assumedQl_(SSU::SSM::None)
    {
        for(int i = 0; i < Vch003DeviceOptions::kNumberOfNetworkConfigurations; ++i) {
            networkConfigurationList_.append(new NetworkConfiguration(this));
        }
    }

    QQmlListProperty<NetworkConfiguration> EthernetPort::networkConfigurationData()
    {
        return QQmlListProperty<NetworkConfiguration>(this, static_cast<void*>(&networkConfigurationList_),
                                                      &EthernetPort::numberOfNetworkConfiguration,
                                                      &EthernetPort::networkConfigurationAt);
    }

    QList<NetworkConfiguration *> EthernetPort::networkConfigurationList() const
    {
        return networkConfigurationList_;
    }

    int EthernetPort::numberOfConfigurations() const
    {
        return networkConfigurationList_.size();
    }

    NetworkConfiguration *EthernetPort::networkConfigurationAt(int i) const
    {
        return i < 0 || i >= networkConfigurationList_.size() ? nullptr
                                                              : networkConfigurationList_.at(i);
    }
    
    EthernetPort::Speed EthernetPort::speed() const
    {
        return speed_;
    }
    
    void EthernetPort::setSpeed(Speed speed)
    {
        if(speed_ != speed) {
            speed_ = speed;
            emit speedChanged();
        }
    }
    
    int EthernetPort::mtu() const
    {
        return mtu_;
    }
    
    void EthernetPort::setMtu(int mtu)
    {
        if(mtu_ != mtu) {
            mtu_ = mtu;
            emit mtuChanged();
        }
    }
    
    bool EthernetPort::swap() const
    {
        return swap_;
    }
    
    void EthernetPort::setSwap(bool swap)
    {
        if(swap_ != swap) {
            swap_ = swap;
            emit swapChanged();
        }
    }
    
    bool EthernetPort::loopback() const
    {
        return loopback_;
    }
    
    void EthernetPort::setLoopback(bool loopback)
    {
        if(loopback_ != loopback) {
            loopback_ = loopback;
            emit loopbackChanged();
        }
    }
    
    bool EthernetPort::syncEMode() const
    {
        return syncEMode_;
    }
    
    void EthernetPort::setSyncEMode(bool syncEMode)
    {
        if(syncEMode_ != syncEMode) {
            syncEMode_ = syncEMode;
            emit syncEModeChanged();
        }
    }
    
    bool EthernetPort::qlMode() const
    {
        return qlMode_;
    }
    
    void EthernetPort::setQlMode(bool qlMode)
    {
        if(qlMode_ != qlMode) {
            qlMode_ = qlMode;
            emit qlModeChanged();
        }
    }
    
    SSU::SSM EthernetPort::assumedQl() const
    {
        return assumedQl_;
    }
    
    void EthernetPort::setAssumedQl(SSU::SSM assumedQl)
    {
        if(assumedQl_ != assumedQl) {
            assumedQl_ = assumedQl;
            emit assumedQlChanged();
        }
    }
    
    EthernetPort::PpsSource EthernetPort::ppsSource() const
    {
        return ppsSource_;
    }
    
    void EthernetPort::setPpsSource(PpsSource ppsSource)
    {
        if(ppsSource_ != ppsSource) {
            ppsSource_ = ppsSource;
            emit ppsSourceChanged();
        }
    }
    
    QString EthernetPort::macAddress() const
    {
        return macAddress_;
    }
    
    void EthernetPort::setMacAddress(const QString &macAddress)
    {
        if(macAddress_ != macAddress) {
            macAddress_ = macAddress;
            emit macAddressChanged();
        }
    }
    
    PortStatistics EthernetPort::portStatistics() const
    {
        return portStatistics_;
    }
    
    void EthernetPort::setPortStatistics(const PortStatistics &portStatistics)
    {
        portStatistics_ = portStatistics;
        emit portStatisticsChanged();
    }

    int EthernetPort::numberOfPortStatistics() const
    {
        return networkConfigurationList_.size();
    }
    
    //    EthernetPort::PtpClockStatus EthernetPort::ptpClockStatus() const
    //    {
    //        return ptpClockStatus_;
    //    }
    
    //    void EthernetPort::setPtpClockStatus(PtpClockStatus ptpClockStatus)
    //    {
    //        if(ptpClockStatus_ != ptpClockStatus) {
    //            ptpClockStatus_ = ptpClockStatus;
    //            emit ptpClockStatusChanged();
    //        }
    //    }
    
    int EthernetPort::numberOfNetworkConfiguration(QQmlListProperty<NetworkConfiguration> *list)
    {
        return static_cast< QList <NetworkConfiguration *>*> (list->data)->size();
    }

    NetworkConfiguration *EthernetPort::networkConfigurationAt(QQmlListProperty<NetworkConfiguration> *list, int i)
    {
        return static_cast< QList <NetworkConfiguration *>*> (list->data)->at(i);
    }
}

