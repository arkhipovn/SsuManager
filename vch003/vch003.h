#ifndef VCH003_H
#define VCH003_H

#include <QObject>
#include <QStringRef>
#include <QQmlEngine>
#include "ssu.h"

// Класс/Пространство имен для Vch003

class Vch003 : public QObject
{
    Q_OBJECT

public:
    static QObject *getInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static void registerTypes();

    enum class DatabaseTable {
        EventLog,
        ActiveEvents,
        HiddenEvents,
        UserActivityLog,
        MeasLog,
        CurrentMeas
    };
    Q_ENUMS(DatabaseTable)

    enum class Sensitivity {
        Sensitivity0    = 0,
        Sensitivity1,
        Sensitivity2,
        Sensitivity3,
        Off             = 255
    };
    Q_ENUMS(Sensitivity)

    enum class E1Bit {
        Sa4,
        Sa5,
        Sa6,
        Sa7,
        Sa8
    };
    Q_ENUMS(E1Bit)

    enum InputSignalState {
        StatusDisqualification              = 1 << 0,        // 1
        LossOfSignal                        = 1 << 1,        // 2            // Дисквалификация по частотному детектору.
        RecoveryDelay                       = 1 << 2,        // 4
        FailureOnTheTransmittingEnd         = 1 << 3,        // 8            // Авария на передающем конце
        AbsenceCRC                          = 1 << 4,        // 16           // отсутствие (несовпадение) CRC
        LossOfMultiFrame                    = 1 << 5,        // 32           // Потеря мультицикловой синхронизации
        LossOfFrame                         = 1 << 6,        // 64           // Потеря цикловой синхронизации
        HDB3Error                           = 1 << 7,        // 128          // ошибка HDB3
        NoFrequencyCapture                  = 1 << 8,        // 256          // нет захвата частоты
    };
    Q_ENUMS(InputSignalState)

    enum class SsmControlMode {
        Auto,
        Manual
    };
    Q_ENUMS(SsmControlMode)

    enum GeneratorError {
        GeneratorFailure            = 1 << 0,        // 1
        DDSFailure                  = 1 << 1,        // 2
        MeasurementSystemFailure    = 1 << 2,        // 4
        ControlLoopFailure          = 1 << 3,        // 8
        PriorityTableError          = 1 << 4         // 16
    };
    Q_ENUMS(GeneratorError)

    enum class DeviceEvent {
        CardIn = 0,
        CartOut,
        ChangedToPassive,
        ChangedToActive,
        ActiveInputChanged,
        PriorityTableError,             // 5

        NoActiveCard,                   // 6    Когда 2 платы активные или 2 пассивные

        WrongSlot = 10,                 // 10
        CardNotFullyInserted,           // 11   Плата не до конца вставлена


        // Input
        DisqualificationBySSM = 100,    // 100
        DisqualificationByStatus,
        LossOfSignal,
        LossOfFrame,
        LossOfMultiframe,               // 104
        SignalAppear,
        SignalDelay,
        InpSSMChanged,                  // 107
        InpTypChanged,                  // 108
        ErrTransmEnd,                   // 109. Авария на передающем конце (бит RAI);
        ErrCRC,                         // 110. Отсутствие (несовпадение) CRC;
        ErrHDB3,                        // 111. Ошибки HDB3;
        ErrFreqCapt,                    // 112. Отсутствие захвата частоты;
        //____

        // Card Output
        BackupInputChanged = 200,       // 200
        ActiveGeneratorFailure,
        BackupGeneratorFailure,
        DisqualificationInternalUSS,    // 203. Сработал частотный детектор на выходной плате
        BackupChannelNoSync,            // 204

        BothChanNoSync,                 // 205. Отсутсвует синх по обоим каналам
        Sync500Fault,                   // 206. Ошибка синхронизации 500 Гц
        Bypass,                         // 207. режим обхода
        OutSSMmodeChanged,              // 208
        OutSSMChanged,                  // 209
        OutManInpChanged,               // 210
        OutTypChanged,                  // 211
        NoMeasMsgs,                     // 212
        ExecCmdChanSwitch,              // 213. Идет переключение ФАПЧ на резервный


        // Card Output only
        DisqualificationByMTIE = 300,   // 300
        DisqualificationByTDEV,
        DisqualificationByDff,

        // Generator
        Warmup  = 400,                   // 400
        Freerun,                        // 401
        Tracking,                       // 402
        Holdover,
        GeneratorERROR,                 // 404

        ActiveSignalFailure,
        GeneratorFailure,               // 406
        DDSFailure,                     // 407
        MeasurmentSystemFailure,
        ControlLoopFailure,             // 409
        Adjustment,                     // 410. Статус генератора
        GModeChanged,                   // 411
        GenManInpChanged,               // 412

        // BM
        PowerAFailure = 500,            // 500
        PowerBFailure,
        FuseAfail,                      // 502. Нет предохранителя в блоке А
        FuseBfail,                      // 503. Нет предохранителя в блоке И
        PowerAout,                      // 504. Нет блока питания А
        PowerBout,                      // 505. Нет блока питания А

        ConnectionLimitExceeded,
        EventLogOverflow,
        MeasurementLogOverflow,
        UserActivityLogOverflow,        // 509

        ConfigChanged,                  // 510. Конфигурация плат изменена

        SignalGenerationError = 600,    // 600

        // Card GNSS
        DisqInput   = 700,              // 700
        GnssModeChanged,                // 701
        GnssNoHold,                     // 702
        GnssAdjust,                     // 703
        GnssHoldover                    // 704
    };
    Q_ENUMS(DeviceEvent)


    Q_INVOKABLE static QString e1bitToString(E1Bit bit);

    Q_INVOKABLE static QString ssmControlModeToString(SsmControlMode mode);

    Q_INVOKABLE static QStringList inputSignalStateToStringList(unsigned char state);
    Q_INVOKABLE static QStringList generatorErrorToStringList(unsigned char error);

    Q_INVOKABLE static QVariantList eventStatusList();  // Вернет список всех возможных событий

private:
    explicit Vch003(QObject *parent = nullptr);
    ~Vch003() = default;
};


#endif // VCH003_H
