#ifndef GENERATORCARD_H
#define GENERATORCARD_H

#include "vch003card.h"
#include "vch003/vch003.h"

namespace ssu {
    class GeneratorCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(int activeChannel READ activeChannel NOTIFY activeChannelChanged)
        Q_PROPERTY(int manualChannel READ manualChannel NOTIFY manualChannelChanged)

        Q_PROPERTY(SSU::WorkingMode mode READ mode NOTIFY modeChanged)

        Q_PROPERTY(SSU::PllState pllState READ pllState NOTIFY pllStateChanged)
        Q_PROPERTY(int pllStateTimer READ pllStateTimer NOTIFY pllStateTimerChanged)

        Q_PROPERTY(int error READ error NOTIFY errorChanged)

        Q_PROPERTY(SSU::SSM ssm READ ssm NOTIFY ssmChanged)

        Q_PROPERTY(bool enableSSMTable READ enableSSMTable NOTIFY enableSSMTableChanged)

        Q_PROPERTY(SSU::GeneratorType generatorType READ generatorType NOTIFY generatorTypeChanged)
        Q_PROPERTY(SSU::SsuType ssuType READ ssuType NOTIFY ssuTypeChanged)
        Q_PROPERTY(bool isMaster READ isMaster NOTIFY masterChanged)

    public:
        explicit GeneratorCard(int address = -1, QObject *parent = nullptr);

        void parseParameters(const QString &parameterList) override;

        int activeChannel() const;
        void setActiveChannel(int channel);

        int manualChannel() const;
        void setManualChannel(int channel);

        SSU::WorkingMode mode() const;
        void setMode(SSU::WorkingMode mode);

        unsigned char error() const;
        void setError(unsigned char error);

        bool enableSSMTable() const;
        void setEnableSSMTable(bool enableSSMTable);

        SSU::SSM ssm() const;
        void setSsm(SSU::SSM ssm);

        SSU::PllState pllState() const;
        void setPllState(SSU::PllState pllState);

        SSU::GeneratorType generatorType() const;
        void setGeneratorType(const SSU::GeneratorType &type);

        bool isMaster() const;
        void setMaster(bool isMaster);

        int pllStateTimer() const;
        void setPllStateTimer(int pllStateTimer);

        SSU::SsuType ssuType() const;
        void setSsuType(const SSU::SsuType &ssuType);

        // Команды управления
        Q_INVOKABLE Request *commandToSetWorkingMode(SSU::WorkingMode mode, int manualChannel = 0);     // выбор режима работы. В случае ручного режима еще нужно считать ind
        Q_INVOKABLE Request *commandToSetActiveSignal(int index);                                       // установка активного сигнала
        Q_INVOKABLE Request *commandToChangeActiveSlot();                                               // смена активного слота
        Q_INVOKABLE Request *commandToSetChannelPriority(int index, int priority);                      // установка приоритета сигнала
        Q_INVOKABLE Request *commandToSetPriorities(const QList <int> &priorities);                     // установка приоритетов входных сигналов
        Q_INVOKABLE Request *enableSSMTableCommand(bool enable);
        Q_INVOKABLE Request *commandToSetSsuType(SSU::SsuType type);

    signals:
        void activeChannelChanged();
        void errorChanged();
        void pllStateChanged();
        void modeChanged();
        void manualChannelChanged();

        void ssmChanged();
        void enableSSMTableChanged();

        void generatorTypeChanged();
        void masterChanged();

        void pllStateTimerChanged();
        void ssuTypeChanged();

    private:
        SSU::GeneratorType generatorType_;  // Тип генератора: кварц/рубидий
        bool master_;                       // Флаг ведущая или ведомая плата
        SSU::SsuType ssuType_;              // Тип SSU (транзитный или локальный)

        SSU::PllState pllState_;
        int pllStateTimer_;                    // Таймер изменения состояния, мин

        int activeChannel_;
        int manualChannel_;
        SSU::WorkingMode mode_;
        unsigned char error_;

        SSU::SSM ssm_;

        bool enableSSMTable_;        // запрет таблицы SSM
    };
}

#endif // GENERATORCARD_H
