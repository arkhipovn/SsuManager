#ifndef INPUTCARD_H
#define INPUTCARD_H

#include "vch003card.h"
#include "vch003/vch003.h"

namespace ssu {
    class InputCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(bool isMaster READ isMaster NOTIFY masterChanged)

    public:
        explicit InputCard(int address = -1, QObject *parent = nullptr);
        void parseParameters(const QString &parameterList) override;

        bool isMaster() const;
        void setMaster(bool master);

        // Команды управления
        Q_INVOKABLE Request *commandToSetTypeChannel(int index, SSU::SignalType type);              // установка типа сигнала
        Q_INVOKABLE Request *commandForAutodetectingChannelType(int index);                         // команда автоопределения типа сигнала
        Q_INVOKABLE Request *commandToSetThresholdStatus(int index, SSU::StatusThreshold status);   // установка установка интерп сигналов внешнего статуса
        Q_INVOKABLE Request *commandToSetSsm(int index, SSU::SSM ssm);                              // установка SSM
        Q_INVOKABLE Request *commandToSetRestoreTime(int index, int sec);                           // установка времени до вост. сигнала
        Q_INVOKABLE Request *commandToSetResetRestoreTimeout(int index);                            // сбросить счетчик ожидания

        // установка чувствительности к ошибкам
        Q_INVOKABLE Request *commandToSetSensitivity(int index,
                                                     Vch003::Sensitivity failureTransmEndSensitivity,
                                                     Vch003::Sensitivity crcErrorsSensitivity,
                                                     Vch003::Sensitivity lossOfMultiFrameSensitivity,
                                                     Vch003::Sensitivity lossOfFrameSensitivity,
                                                     Vch003::Sensitivity hdb3ErrorSensitivity);

        Q_INVOKABLE Request *commandToSetSensitivityToFailureAtTheTransmEnd(int index, Vch003::Sensitivity sensitivity);
        Q_INVOKABLE Request *commandToSetSensitivityToCrcErrors(int index, Vch003::Sensitivity sensitivity);
        Q_INVOKABLE Request *commandToSetSensitivityToLossOfMultiframeSync(int index, Vch003::Sensitivity sensitivity);
        Q_INVOKABLE Request *commandToSetSensitivityToLossOfFrameSync(int index, Vch003::Sensitivity sensitivity);
        Q_INVOKABLE Request *commandToSetSensitivityHdb3coddingError(int index, Vch003::Sensitivity sensitivity);

        Q_INVOKABLE Request *commandToSetE1Bit(int index, Vch003::E1Bit bit);                   // бит для передачи ssm
        Q_INVOKABLE Request *commandToSetMeasurementMode(int index, bool measurementMode);      // режим измерений

    signals:
        void masterChanged();

    protected:
        bool master_;               // Флаг ведущая или ведомая плата
    };
}

#endif // INPUTCARD_H

