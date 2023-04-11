#ifndef INCHANNELINPUTCARD_H
#define INCHANNELINPUTCARD_H

#include "device/panel/channel.h"
#include "vch003/vch003.h"

namespace ssu {
    class InChannelInputCard : public Channel
    {
        Q_OBJECT

        Q_PROPERTY(bool ban READ ban NOTIFY banChanged)
        Q_PROPERTY(SSU::SignalType type READ type NOTIFY typeChanged)
        Q_PROPERTY(SSU::StatusThreshold statusThreshold READ statusThreshold NOTIFY statusThresholdChanged)
        Q_PROPERTY(SSU::SSM ssm READ ssm NOTIFY ssmChanged)
        Q_PROPERTY(int waitToRestoreTime READ waitToRestoreTime NOTIFY waitToRestoreTimeChanged)
        Q_PROPERTY(int state READ state NOTIFY stateChanged)

        Q_PROPERTY(bool measurementMode READ measurementMode NOTIFY measurementModeChanged)
        Q_PROPERTY(Vch003::E1Bit e1bit READ e1bit NOTIFY e1bitChanged)

        Q_PROPERTY(Vch003::Sensitivity failureTransmEndSensitivity READ failureTransmEndSensitivity NOTIFY failureTransmEndSensitivityChanged)
        Q_PROPERTY(Vch003::Sensitivity crcErrorsSensitivity READ crcErrorsSensitivity NOTIFY crcErrorsSensitivityChanged)
        Q_PROPERTY(Vch003::Sensitivity lossOfMultiFrameSensitivity READ lossOfMultiFrameSensitivity NOTIFY lossOfMultiFrameSensitivityChanged)
        Q_PROPERTY(Vch003::Sensitivity lossOfFrameSensitivity READ lossOfFrameSensitivity NOTIFY lossOfFrameSensitivityChanged)
        Q_PROPERTY(Vch003::Sensitivity hdb3ErrorSensitivity READ hdb3ErrorSensitivity NOTIFY hdb3ErrorSensitivityChanged)

    public:
        explicit InChannelInputCard(QObject *parent = nullptr);

        bool ban() const;
        void setBan(bool ban);

        int state() const;
        void setState(int state);

        SSU::SignalType type() const;
        void setType(const SSU::SignalType &type);

        SSU::StatusThreshold statusThreshold() const;
        void setStatusThreshold(SSU::StatusThreshold statusThreshold);

        SSU::SSM ssm() const;
        void setSsm(SSU::SSM ssm);

        int waitToRestoreTime() const;
        void setWaitToRestoreTime(int waitToRestoreTime);

        bool measurementMode() const;
        void setMeasurementMode(bool measurementMode);

        Vch003::E1Bit e1bit() const;
        void setE1bit(Vch003::E1Bit e1bit);

        Vch003::Sensitivity failureTransmEndSensitivity() const;
        void setFailureTransmEndSensitivity(Vch003::Sensitivity failureTransmEndSensitivity);

        Vch003::Sensitivity crcErrorsSensitivity() const;
        void setCrcErrorsSensitivity(Vch003::Sensitivity crcErrorsSensitivity);

        Vch003::Sensitivity lossOfMultiFrameSensitivity() const;
        void setLossOfMultiFrameSensitivity(Vch003::Sensitivity lossOfMultiFrameSensitivity);

        Vch003::Sensitivity lossOfFrameSensitivity() const;
        void setLossOfFrameSensitivity(Vch003::Sensitivity lossOfFrameSensitivity);

        Vch003::Sensitivity hdb3ErrorSensitivity() const;
        void setHdb3ErrorSensitivity(Vch003::Sensitivity hdb3ErrorSensitivity);

    signals:
        void banChanged();
        void stateChanged();
        void typeChanged();
        void statusThresholdChanged();
        void ssmChanged();
        void waitToRestoreTimeChanged();
        void measurementModeChanged();
        void e1bitChanged();

        void failureTransmEndSensitivityChanged();
        void crcErrorsSensitivityChanged();
        void lossOfMultiFrameSensitivityChanged();
        void lossOfFrameSensitivityChanged();
        void hdb3ErrorSensitivityChanged();

    private:
        bool ban_;
        int state_;

        SSU::SignalType type_;

        SSU::StatusThreshold statusThreshold_;  // Пороговые значения по статусу
        SSU::SSM ssm_;

        int waitToRestoreTime_;
        bool measurementMode_;     // Режим измерений

        Vch003::E1Bit e1bit_;

        Vch003::Sensitivity failureTransmEndSensitivity_;
        Vch003::Sensitivity crcErrorsSensitivity_;
        Vch003::Sensitivity lossOfMultiFrameSensitivity_;
        Vch003::Sensitivity lossOfFrameSensitivity_;
        Vch003::Sensitivity hdb3ErrorSensitivity_;
    };
}

#endif // INCHANNELINPUTCARD_H
