#ifndef ETHERNETPORTMODEL_H
#define ETHERNETPORTMODEL_H

#include <QObject>
#include "ssu.h"
#include "networkconfiguration.h"
#include "portstatistics.h"

namespace ssu {

    class EthernetPort : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(PortStatistics statistics READ portStatistics NOTIFY portStatisticsChanged)

        Q_PROPERTY(QQmlListProperty <NetworkConfiguration> networkConfigurationList READ networkConfigurationData CONSTANT)    //Channel

        Q_PROPERTY(Speed speed READ speed NOTIFY speedChanged)
        Q_PROPERTY(int mtu READ mtu NOTIFY mtuChanged)

        Q_PROPERTY(bool swap READ swap NOTIFY swapChanged)
        Q_PROPERTY(bool loopback READ loopback NOTIFY loopbackChanged)
        Q_PROPERTY(bool syncEMode READ syncEMode NOTIFY syncEModeChanged)
        Q_PROPERTY(bool qlMode READ qlMode NOTIFY qlModeChanged)

        Q_PROPERTY(SSU::SSM assumedQl READ assumedQl NOTIFY assumedQlChanged)

        Q_PROPERTY(PpsSource ppsSource READ ppsSource NOTIFY ppsSourceChanged)
        //        Q_PROPERTY(PtpClockStatus ptpClockStatus READ ptpClockStatus NOTIFY ptpClockStatusChanged)

        Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)

    public:
        explicit EthernetPort(QObject *parent = nullptr);

        enum class Speed {
            Invalid,
            Auto,
            NoCapture,
            M1000,
            M100,
            M10
        };
        Q_ENUMS(Speed)

        enum class PpsSource {
            Invalid,
            MasterInternal,
            MasterGnss,
            MasterSlave,
            Slave
        };
        Q_ENUMS(PpsSource)

        //        enum PtpClockStatus {
        //            Lock,
        //            NotLock
        //        };
        //        Q_ENUMS(PtpClockStatus)

        QQmlListProperty <NetworkConfiguration> networkConfigurationData();
        QList<NetworkConfiguration *> networkConfigurationList() const;

        int numberOfConfigurations() const;
        NetworkConfiguration *networkConfigurationAt(int i) const;

        Speed speed() const;
        void setSpeed(Speed speed);

        int mtu() const;
        void setMtu(int mtu);

        bool swap() const;
        void setSwap(bool swap);

        bool loopback() const;
        void setLoopback(bool loopback);

        bool syncEMode() const;
        void setSyncEMode(bool syncEMode);

        bool qlMode() const;
        void setQlMode(bool qlMode);

        SSU::SSM assumedQl() const;
        void setAssumedQl(SSU::SSM assumedQl);

        PpsSource ppsSource() const;
        void setPpsSource(PpsSource ppsSource);

        //        PtpClockStatus ptpClockStatus() const;
        //        void setPtpClockStatus(PtpClockStatus ptpClockStatus);

        QString macAddress() const;
        void setMacAddress(const QString &macAddress);

        PortStatistics portStatistics() const;
        void setPortStatistics(const PortStatistics &portStatistics);

        int numberOfPortStatistics() const;

    signals:
        void macAddressChanged();

        void speedChanged();
        void mtuChanged();
        void swapChanged();
        void loopbackChanged();

        void syncEModeChanged();
        void qlModeChanged();
        void assumedQlChanged();

        void ppsSourceChanged();
        //        void ptpClockStatusChanged();

        void portStatisticsChanged();

    private:
        PortStatistics portStatistics_;

        PpsSource ppsSource_;
        //        PtpClockStatus ptpClockStatus_;

        QString macAddress_;

        // Port config
        Speed speed_;               // NoConnection/10/100/1000
        int mtu_;                   // 0 - 9600
        bool swap_;                 // Disabled/Enabled
        bool loopback_;             // Disabled/Enabled

        // SyncE Configuration
        bool syncEMode_;            // Disabled/Enabled
        bool qlMode_;               // Disabled/Enabled
        SSU::SSM assumedQl_;        // PRC/SSU-A/SSU-B/EEC1/None

        QList <NetworkConfiguration *> networkConfigurationList_;       // Список конфигураций

        static int numberOfNetworkConfiguration(QQmlListProperty <NetworkConfiguration> *list);
        static NetworkConfiguration* networkConfigurationAt(QQmlListProperty <NetworkConfiguration> *list, int i);
    };
}

#endif // ETHERNETPORTMODEL_H
