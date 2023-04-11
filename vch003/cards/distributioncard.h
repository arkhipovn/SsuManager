#ifndef DISTRIBUTIONCARD_H
#define DISTRIBUTIONCARD_H

#include "vch003card.h"
#include "vch003/vch003.h"

namespace ssu {
    class DistributionCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(bool active READ active NOTIFY activeChanged)
        Q_PROPERTY(bool synchronization READ synchronization NOTIFY synchronizationChanged)

        Q_PROPERTY(bool enableOutputChannels READ enableOutputChannels NOTIFY enableOutputChannelsChanged)

    public:
        explicit DistributionCard(int address = -1, QObject *parent = nullptr);
        void parseParameters(const QString &parameterList) override;

        bool active() const;
        void setActive(bool active);

        bool synchronization() const;
        void setSynchronization(bool synchronization);

        bool enableOutputChannels() const;
        void setEnableOutputChannels(bool enableOutputChannels);

        // Команды управления
        Q_INVOKABLE Request *commandToGiveActivity();                                                   // Отдать активность
        Q_INVOKABLE Request *commandToSetTypeOutputChannel(int index, SSU::SignalType type);            // Установка типа сигнала
        Q_INVOKABLE Request *commandToSetPermissionOnHoldover(int index, bool check);

    signals:
        void activeChanged();
        void synchronizationChanged();
        void enableOutputChannelsChanged();

    private:
        bool active_;
        bool synchronization_;   // 500 гц

        bool enableOutputChannels_;      // выдается ли сигнал с выходов
    };
}

#endif // DISTRIBUTIONCARD_H
