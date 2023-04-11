#ifndef SSU_H
#define SSU_H

#include <QObject>
#include <QStringRef>
#include <QQmlEngine>

#include "data/datacontainer.h"
#include "global.h"

// Класс/Пространство имен с константами

//@N namespace   qmlRegisterUncreatableMetaObject
//@N Q_GADGET register..

class SSU : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool kLedTesting MEMBER kLedTesting CONSTANT)
    Q_PROPERTY(bool kImitationWork MEMBER kImitationWork CONSTANT)

public:
    static QObject *getInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static void registerTypes();

    // Program settings
    static constexpr bool kImitationWork                   = true;
    static constexpr bool kLedTesting                      = kImitationWork;        // Тестирование светодиодов
    static constexpr bool kChangedParamImmediately         = true;                  // Изменять параметры плат сразу же при установке?

    Q_INVOKABLE static QString maskDataDirPath();                                   // полный путь до папки с масками
    Q_INVOKABLE static QString deviceBackupDirPath();                               // полный путь до папки с резервными копиями устройства
    Q_INVOKABLE static QString windowSettingsGroup();                               // группа с настройками отображения

    Q_INVOKABLE static QString programSettingsFile();                               // файл с настройками программы
    Q_INVOKABLE static QString programDataPath();                                   // полный путь до папки с данными
    Q_INVOKABLE static QString programDatabaseFile(const QString &deviceDir);       // файл базы данных
    Q_INVOKABLE static QString downloadTempFile();                                  // временного бекап файл

    static constexpr int kMonitoringThreadSleep                 = 500;          // если нет команд
    static constexpr int kCommandSendingInterval                = 20;           // мс, интервал таймера отправки команд
    static constexpr int kAuthorisationSendingInterval          = 250;          // мс, интервал таймера отправки команд авторизации
    static constexpr int kReconnectInterval                     = 2000;         // мс, интервал подключения
    static constexpr int kResponseTime                          = 4000;         // мс, время ожидания ответа от девайса при подключении
    static constexpr int kLogReadResponseTime                   = 120000;       // мс       //@N мб сделать зависимым от кол-во дней при считывании..
    static constexpr int kFileReadResponseTime                  = 120000;       // мс
    static constexpr int kMutexTimeout                          = 2000;         // мc

    static constexpr int kFileWatcherCheckInterval              = 1000;         // ms
    static constexpr int kFileWatcherMaskListCheckInterval      = 10000;        // ms
    static constexpr int kFileWatcherProgramDataCheckInterval   = 10000;        // ms

    static constexpr int kDelayBeforeCheckRequest               = 3000;     // ms, после того как что-то отправили запросим изменения через
    static constexpr int kDelayForSendingRequest                = 1000;     // ms, после того как что-то изменилось опросим через

    static constexpr int kMaxNumberOfAttemptsToSendMessages     = 3;        // максимальное количество попыток отправки сообщения
    static constexpr int kMaxNumberOfConnectAttempts            = 3;        // максимальное количество попыток соединения
    static constexpr int kMaxNumberOfReconnectAttempts          = 100;      // максимальное количество попыток переподключения
    static constexpr int kMaxNumberOfCommands                   = 300;      // максимальное кол-во команд в списке


    static constexpr int kMaxNumberOfSecToUpdateLogData         = 3600;       // максимальное количество секунд при обновлении данных журнала
    static constexpr int kMaxMessageSize                        = 1024 * 1024;   // максимальной размер посылки. В байтах
    static constexpr int kMaxNumberOfLinesToLoadTheDatabase     = 50000;        // максимальное кол-во строк для загрузки в базу данных


    enum class SsuType {
        SSU_A,      // Транзитный
        SSU_B       // Локальный
    };
    Q_ENUMS(SsuType)

    enum class DeviceType {
        None    = 0,
        Vch003  = 1
    };
    Q_ENUMS(DeviceType)

    enum CardType {
        InvalidType = 0,
        Input,
        Output,
        OutputH,
        OutputL,
        Distribution,   // 5
        Generator,
        Monitoring,
        Gnss,
        Power,
        Stub
    };
    Q_ENUMS(CardType)

    enum class MeasDataType {
        Mtie,
        Tdev,
        Rf,             // Relative freq. df/f
        Invalid = 255
    };
    Q_ENUMS(MeasDataType)

    enum class UserLevel {
        User = 0,
        Admin,
        Security
    };
    Q_ENUMS(UserLevel)

    enum class SatelliteSystem {
        UnknownSystem   = 0,
        GPS,
        GLONASS,
        SBASpayload,
        Galileo,
        BeiDou,
        QZSS,
        IRNSS
    };
    Q_ENUMS(SatelliteSystem)

    enum class SSM {
        None = 0,           // 0000
        PRC,                // 0010
        SSU_T,              // 0100
        SSU_L,              // 1000
        SEC,                // 1011
        DNU                 // 1111
    };
    Q_ENUMS(SSM)

    enum class SignalType {
        Off = 0,
        MHz1,
        KHz2048,
        MHz5,
        MHz10,
        E1,
        Auto
    };
    Q_ENUMS(SignalType)

    enum class CardMode {
        Active,
        Passive
    };
    Q_ENUMS(CardMode)

    enum class EventStatus {
        Critical = 0,
        Major,
        Minor,
        Warning,
        NotAlarm,
        Info
    };
    Q_ENUMS(EventStatus)

    enum class StatusThreshold {
        None = 0,
        Low,
        High
    };
    Q_ENUMS(StatusThreshold)

    enum class WorkingMode {    // GeneratorMode
        AutoMode,
        ManualMode,
        ForcedHoldover
    };
    Q_ENUMS(WorkingMode)

    enum class PllState {
        Warmup = 0,
        Freerun,
        Tracking,
        Holdover,
        Adjustment,
        Error,
        NoCapture
    };
    Q_ENUMS(PllState)

    enum class Hemisphere {
        North,
        West,
        South,
        East
    };

    Q_ENUMS(Hemisphere)

    enum class GeneratorType {
        Quartz,
        Rubidium
    };

    Q_ENUMS(GeneratorType)

    enum class ControlMode {    //@N как правильно назвать?   // PtpMode
        Master,
        Slave
    };

    Q_ENUMS(ControlMode)



    //@N Мб большую часть перенести в файл js?

    Q_INVOKABLE static QString generatorTypeToString(GeneratorType type);
    Q_INVOKABLE static QString hemisphereToString(Hemisphere hemisphere);
    Q_INVOKABLE static QString pllStateToString(PllState state);
    Q_INVOKABLE static QString workingModeToString(WorkingMode mode);
    Q_INVOKABLE static QString satelliteSystemToString(SatelliteSystem system);
    Q_INVOKABLE static QString shortSatelliteSystemToString(SatelliteSystem system);
    Q_INVOKABLE static QString cardTypeToString(CardType type);
    Q_INVOKABLE static QString measDataTypeToString(MeasDataType type);
    Q_INVOKABLE static QString userLevelToString(UserLevel level);
    Q_INVOKABLE static QString ssmToString(SSM ssm);
    Q_INVOKABLE static QString signalTypeToString(SignalType type);
    Q_INVOKABLE static QString statusThresholdToString(StatusThreshold status);
    Q_INVOKABLE static QString eventStatusToString(EventStatus status);

    Q_INVOKABLE static QString addressToString(int address);
    Q_INVOKABLE static int addressFromString(const QString &address);
    Q_INVOKABLE static int addressFromString(const QStringRef &address);

    Q_INVOKABLE static QString controlModeToString(SSU::ControlMode value);

    Q_INVOKABLE static ssu::DataContainer createEmptyData();    //@N куда-нибудь убрать.. Вообще переделать бы все это

    Q_INVOKABLE static QVariantList eventStatusList();  // Вернет список всех возможных событий


protected:
    explicit SSU(QObject *parent = nullptr);
    ~SSU() = default;
};



#endif // SSU_H
