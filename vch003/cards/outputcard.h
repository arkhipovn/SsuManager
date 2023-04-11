#ifndef OUTPUTCARD_H
#define OUTPUTCARD_H

#include "vch003card.h"
#include "vch003/vch003.h"

namespace ssu {
    class OutputCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(bool active READ active NOTIFY activeChanged)
        Q_PROPERTY(bool synchronization READ synchronization NOTIFY synchronizationChanged)

        Q_PROPERTY(Vch003::SsmControlMode ssmControlMode READ ssmControlMode NOTIFY ssmControlModeChanged)
        Q_PROPERTY(SSU::SSM ssm READ ssm NOTIFY ssmChanged)
        Q_PROPERTY(bool ssmRai READ ssmRai NOTIFY ssmRaiChanged)

        Q_PROPERTY(int activeChannel READ activeChannel NOTIFY activeChannelChanged)
        Q_PROPERTY(int backupChannel READ backupChannel NOTIFY backupChannelChanged)

        Q_PROPERTY(bool enableSSMTable READ enableSSMTable NOTIFY enableSSMTableChanged)
        Q_PROPERTY(bool enableBypass READ enableBypass NOTIFY enableBypassChanged)

        Q_PROPERTY(bool enableOutputChannels READ enableOutputChannels NOTIFY enableOutputChannelsChanged)

    public:
        explicit OutputCard(int address = -1, QObject *parent = nullptr);
        void parseParameters(const QString &parameterList) override;

        bool active() const;
        void setActive(bool active);

        bool synchronization() const;
        void setSynchronization(bool synchronization);

        int activeChannel() const;
        void setActiveChannel(int activeChannel);

        int backupChannel() const;
        void setBackupChannel(int backupChannel);

        Vch003::SsmControlMode ssmControlMode() const;
        void setSsmControlMode(Vch003::SsmControlMode ssmControlMode);

        SSU::SSM ssm() const;
        void setSsm(SSU::SSM ssm);

        bool ssmRai() const;
        void setSsmRai(bool ssmRai);

        bool enableSSMTable() const;
        void setEnableSSMTable(bool enableSSMTable);

        bool enableBypass() const;
        void setEnableBypass(bool enableBypass);

        bool enableOutputChannels() const;
        void setEnableOutputChannels(bool enableOutputChannels);

        // Команды управления
        Q_INVOKABLE Request *commandToSetTypeOutputChannel(int index, SSU::SignalType type);        // Установка типа сигнала

        Q_INVOKABLE Request *commandToGiveActivity();                                               // Отдать активность
        Q_INVOKABLE Request *commandToSwitchToBackupGenerator();                                    // Переключиться на парный генератор

        Q_INVOKABLE Request *commandToSetPriorityInputSignal(int index, int priority);              // Установка приоритета сигнала
        Q_INVOKABLE Request *commandToSetPriorities(const QList <int> &priorities);                 // Установка приоритетов входных сигналов

        Q_INVOKABLE Request *commandToSetSsmControlMode(Vch003::SsmControlMode ssmControlMode);     // Управление ssm
        Q_INVOKABLE Request *commandToSetSsmRai(bool ssmRai);                                       // Установка бита Rai
        Q_INVOKABLE Request *commandToSetSsm(SSU::SSM ssm);                                         // Установка SSM

        Q_INVOKABLE Request *enableSSMTableCommand(bool enable);
        Q_INVOKABLE Request *enableBypassCommand(bool enable);

        Q_INVOKABLE Request *commandToSetPermissionOnHoldover(int index, bool check);

    signals:
        void activeChanged();
        void synchronizationChanged();
        void activeChannelChanged();
        void backupChannelChanged();

        void ssmControlModeChanged();
        void ssmChanged();
        void ssmRaiChanged();

        void enableSSMTableChanged();
        void enableBypassChanged();
        void enableOutputChannelsChanged();

    private:
        bool active_;
        bool synchronization_;   // 500 гц

        int activeChannel_;
        int backupChannel_;

        Vch003::SsmControlMode ssmControlMode_;
        SSU::SSM ssm_;
        bool ssmRai_;

        bool enableSSMTable_;           // запрет таблицы SSM
        bool enableBypass_;
        bool enableOutputChannels_;      // выдается ли сигнал с выходов
    };
}

#endif // OUTPUTCARD_H
