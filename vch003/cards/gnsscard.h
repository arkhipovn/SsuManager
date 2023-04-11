#ifndef GNSSCARD_H
#define GNSSCARD_H

#include "vch003card.h"
#include "vch003/vch003.h"

#include "gnss/satellitelistmodel.h"
#include "gnss/gnssmodule.h"

#include "timeprotocol/ethernetport.h"

namespace ssu {
    class GnssCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(bool ban READ ban NOTIFY banChanged)

        Q_PROPERTY(bool measurementMode READ measurementMode NOTIFY measurementModeChanged)
        Q_PROPERTY(int delayToRemoveBanByMeasurements READ delayToRemoveBanByMeasurements NOTIFY delayToRemoveBanByMeasurementsChanged)

        Q_PROPERTY(GnssModule *gnss READ gnss CONSTANT)
        Q_PROPERTY(SatelliteListModel *satelliteList READ satelliteList CONSTANT)

        Q_PROPERTY(int referenceGeneratorAddress READ referenceGeneratorAddress NOTIFY referenceGeneratorAddressChanged)

        Q_PROPERTY(int activeChannel READ activeChannel NOTIFY activeChannelChanged)
        Q_PROPERTY(int manualChannel READ manualChannel NOTIFY manualChannelChanged)

        Q_PROPERTY(SSU::WorkingMode mode READ mode NOTIFY modeChanged)

        Q_PROPERTY(SSU::PllState pllState READ pllState NOTIFY pllStateChanged)
        Q_PROPERTY(int pllStateTimer READ pllStateTimer NOTIFY pllStateTimerChanged)

        Q_PROPERTY(SSU::SSM ssm READ ssm NOTIFY ssmChanged)

        Q_PROPERTY(EthernetPort *port0 READ port0 CONSTANT)
        Q_PROPERTY(EthernetPort *port1 READ port1 CONSTANT)

    public:
        explicit GnssCard(int address = -1, QObject *parent = nullptr);
        void parseParameters(const QString &parameterList) override;

        void parsePortSettings(int portIndex, const QString &parameterList);
        void parsePortStatistics(int portIndex, const QString &parameterList);
        void parseNetworkConfiguration(int portIndex, int confIndex, const QString &parameterList);

        QList <Request*> requestList() const override;
        QList <Request*> initCommandList() const override;

        SatelliteListModel *satelliteList() const;

        int activeChannel() const;
        void setActiveChannel(int channel);

        int manualChannel() const;
        void setManualChannel(int channel);

        SSU::WorkingMode mode() const;
        void setMode(SSU::WorkingMode mode);

        SSU::PllState pllState() const;
        void setPllState(SSU::PllState pllState);

        SSU::SSM ssm() const;
        void setSsm(SSU::SSM ssm);

        GnssModule *gnss() const;

        bool ban() const;
        void setBan(bool ban);

        EthernetPort *port0() const;
        EthernetPort *port1() const;

        bool measurementMode() const;
        void setMeasurementMode(bool measurementMode);

        int delayToRemoveBanByMeasurements() const;
        void setDelayToRemoveBanByMeasurements(int delayToRemoveBanByMeasurements);

        int pllStateTimer() const;
        void setPllStateTimer(int pllStateTimer);

        int referenceGeneratorAddress() const;
        void setReferenceGeneratorAddress(int referenceGeneratorAddress);

        // Команды управления
        Q_INVOKABLE Request *updateSatelliteListCommand() const;
        Q_INVOKABLE Request *updatePortSettingsCommand(EthernetPort *port) const;
        Q_INVOKABLE Request *updatePortStatisticsCommand(EthernetPort *port) const;
        Q_INVOKABLE Request *updateNetworkConfigurationCommand(EthernetPort *port, int confIndex) const;

        Q_INVOKABLE Request *commandToSetWorkingMode(SSU::WorkingMode mode, int manualChannel = 0);     // выбор режима работы

        Q_INVOKABLE Request *commandToSetActiveSignal(int index);                                       // установка активного сигнала
        Q_INVOKABLE Request *commandToSetChannelPriority(int index, int priority);                      // установка приоритета сигнала
        Q_INVOKABLE Request *commandToSetPriorities(const QList <int> &priorities);                     // установка приоритетов входных сигналов

        Q_INVOKABLE Request *commandToSetSsm(SSU::SSM ssm);                                             // установка SSM
        Q_INVOKABLE Request *commandToSetMeasurementMode(bool measurementMode);                         // режим измерений
        Q_INVOKABLE Request *commandToSetDelayToRemoveBanByMeasurements(bool sec);                      // Задержка на снятие БАНа по измерениям

        Q_INVOKABLE Request *commandToChannelConfiguration(int index, bool configuration);              // конфигурация входов. Вкл/выкл
        Q_INVOKABLE Request *commandToSetRestoreTime(int index, int sec);                               // установка времени до вост. сигнала
        Q_INVOKABLE Request *commandToSetResetRestoreTimeout();                                         // сбросить счетчик ожидания

        Q_INVOKABLE Request *commandToSetPdopMask(double mask);
        Q_INVOKABLE Request *commandToSetAntennaCableDelay(int delay);

        Q_INVOKABLE Request *commandToSetSnr(int count);
        Q_INVOKABLE Request *commandToSetSnrMask(int mask);
        Q_INVOKABLE Request *commandToSetElevationMask(int mask);

        Q_INVOKABLE Request *commandToSelectSystems(const QList <int> &systems);

        // Port
        Q_INVOKABLE Request *commandToSetPpsSource(EthernetPort *port, EthernetPort::PpsSource source);
        Q_INVOKABLE Request *commandToSetPortMacAddress(EthernetPort *port, QString value);
        Q_INVOKABLE Request *commandToSetPortSpeed(EthernetPort *port, EthernetPort::Speed value);
        Q_INVOKABLE Request *commandToSetPortMtu(EthernetPort *port, int value);
        Q_INVOKABLE Request *commandToSetPortSwap(EthernetPort *port, bool value);
        Q_INVOKABLE Request *commandToSetPortLoopback(EthernetPort *port, bool value);
        //        Q_INVOKABLE Request *commandToSetPortSencEMode(EthernetPort *port, bool value);
        //        Q_INVOKABLE Request *commandToSetPortQlMode(EthernetPort *port, bool value);
        //        Q_INVOKABLE Request *commandToSetPortAssumedQl(EthernetPort *port, SSU::SSM value);


        // Configuration
        Q_INVOKABLE Request *networkConfigurationCommand(EthernetPort *port, int index,
                                                         const QString &ip, const QString &ipMask, const QString &gateway);
        Q_INVOKABLE Request *ntpConfigurationCommand(EthernetPort *port, int index, int stratum, const QString &serverRefId);

        Q_INVOKABLE Request *ptpConfigurationCommand(EthernetPort *port, int index,
                                                     Ptp::TxMode txMode, Ptp::WayMode wayMode, Ptp::SyncMode syncMode,
                                                     Ptp::DelayMechanism delayMech, int domainNumber, Ptp::Protocol protocol,
                                                     Ptp::PtpMode ptpMode, const QString clockId,
                                                     int announceTimeout, int syncInterval, int delayRequetInterval, int pDelayRequestInterval,
                                                     int priority1, int priority2, int clockClass, const QString removeMasterAddress);


        Q_INVOKABLE Request *clearConfigurationCommand(EthernetPort *port, int index);
        Q_INVOKABLE Request *clearNtpConfigurationCommand(EthernetPort *port, int index);
        Q_INVOKABLE Request *clearPtpConfigurationCommand(EthernetPort *port, int index);
        Q_INVOKABLE Request *clearPortStatisticsCommand(EthernetPort *port);

    signals:
        void activeChannelChanged();
        void modeChanged();
        void manualChannelChanged();

        void pllStateChanged();
        void pllStateTimerChanged();

        void ssmChanged();

        void banChanged();
        void measurementModeChanged();
        void delayToRemoveBanByMeasurementsChanged();

        void referenceGeneratorAddressChanged();

    private:
        bool ban_;	// BAN от ПЛИС
        bool measurementMode_;                  // Режим измерений
        int delayToRemoveBanByMeasurements_;    // Задержка на снятие БАНа по измерениям

        SSU::PllState pllState_;
        int pllStateTimer_;                     // Таймер изменения состояния, мин

        // Блок ГНСС синхронизируется с блоком Генератора. В этой переменной будет адресс блока Генератора
        // -1 - нет опоры.
        int referenceGeneratorAddress_;

        int activeChannel_;
        int manualChannel_;
        SSU::WorkingMode mode_;

        SSU::SSM ssm_;

        SatelliteListModel *satelliteList_;

        GnssModule *gnss_;

        EthernetPort *port0_;
        EthernetPort *port1_;

        int portNumber(EthernetPort *port) const;
        EthernetPort *portAt(int index) const;
    };
}

#endif // GNSSCARD_H



