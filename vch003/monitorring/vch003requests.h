#ifndef VCH003REQUESTS_H
#define VCH003REQUESTS_H

#include <QDateTime>

#include "request.h"
#include "ssu.h"
#include "filerequest.h"
#include "device/led/led.h"
#include "vch003/vch003.h"
#include "maskdata/maskdata.h"
#include "vch003/cards/timeprotocol/ethernetport.h"

namespace ssu {
#define GREETING_MESSAGE                QString("[\\r\\n]+.*-> ")      // "^\\s+.*-> $"
#define LOGIN_REQUEST_MESSAGE           QString("Login: ")
#define PASSWORD_REQUEST_MESSAGE        QString("Password: ")
#define BACKUP_REQUEST_MESSAGE          QByteArray("File size")
#define BACKUP_HASH_MESSAGE             QByteArray("File hash")

#define DEVICE_TIME_FORMAT              "dd.MM.yyyy HH:mm:ss"
#define REGEXP_TIME                     QString("\\d{2}.\\d{2}.\\d{4} \\d{2}:\\d{2}:\\d{2}")
#define REGEXP_FLOAT                    QString("-?\\d+\\.?\\d*e?[+-]?\\d*")

    //@N переделать бы все, что связано с устройством (Device)..

    class Device;
    namespace vch003request {
        //------- Список инветаризации -------
        class GetInventory : public Request
        {
        public:
            static QString cmd() { return "get inventory"; }
            GetInventory();
        };



        //------- Получение параметров платы -------
        class GetCardParameters : public Request
        {
        public:
            static QString cmd() { return "get prm"; }
            GetCardParameters(const QString &slot = "");
        };

        //------- Установка параметров платы -------
        class SetCardParameter : public Request
        {
        public:
            SetCardParameter(const QString &slot, const QString &parameter, const QString &value = "");
            SetCardParameter(const QString &slot, const QString &parameter, const QStringList &values);
        };


        //------- Получение списка спутников -------
        class GetSatelliteList : public Request
        {
        public:
            static QString cmd() { return "get stls"; }
            GetSatelliteList(const QString &slot);
        };

        //------- Получение настроек порта в ГНСС плате -------
        class GetGnssPortSettings : public Request
        {
        public:
            static QString cmd() { return "get gnprt"; }
            GetGnssPortSettings(const QString &slot, int portIndex);
        };

        //------- Получение статистики порта в ГНСС плате -------
        class GetGnssPortStatistics : public Request
        {
        public:
            static QString cmd() { return "get prtstat"; }
            GetGnssPortStatistics(const QString &slot, int portIndex);
        };

        //------- Получение конфигурации в ГНСС плате -------
        class GetGnssNetworkConfiguration : public Request
        {
        public:
            static QString cmd() { return "get netcfg"; }
            GetGnssNetworkConfiguration(const QString &slot, int portIndex, int confIndex);
        };

        //------- Очистка конфигурации в ГНСС плате -------
        class ClearGnssPortConfiguration : public Request
        {
        public:
            static QString cmd() { return ""; } //@N add command
            ClearGnssPortConfiguration(const QString &slot, int portIndex, int confIndex);
        };

        //------- Очистка конфигурации в ГНСС плате -------
        class ClearNtpConfiguration : public Request
        {
        public:
            static QString cmd() { return ""; } //@N add command
            ClearNtpConfiguration(const QString &slot, int portIndex, int confIndex);
        };

        //------- Очистка конфигурации в ГНСС плате -------
        class ClearPtpConfiguration : public Request
        {
        public:
            static QString cmd() { return ""; } //@N add command
            ClearPtpConfiguration(const QString &slot, int portIndex, int confIndex);
        };

        //------- Очистка конфигурации в ГНСС плате -------
        class ClearGnssPortStatistics : public Request
        {
        public:
            static QString cmd() { return ""; } //@N add command
            ClearGnssPortStatistics(const QString &slot, int portIndex);
        };


        //------- Сброс платы -------
        class ResetCard : public Request
        {
        public:
            static QString cmd() { return "set reset"; }
            ResetCard(const QString &slot = "");
        };

        //------- Возврат к настройкам по умолчанию -------
        class ResetToDefaultCard : public Request
        {
        public:
            static QString cmd() { return "set resdefault"; }
            ResetToDefaultCard(const QString &slot = "");
        };

        //------- Список активных событий -------
        class GetActiveEvents : public Request
        {
        public:
            static QString cmd() { return "get alarms"; }
            GetActiveEvents();
        };

        //------- Получить список возможных событий у платы -------
        class GetEventDescription : public Request
        {
        public:
            static QString cmd() { return "get events"; }
            GetEventDescription();
            GetEventDescription(SSU::CardType card);
        };

        //------- Установить стутус событию -------
        class SetCardEventStatus : public Request
        {
        public:
            static QString cmd() { return "set event"; }
            SetCardEventStatus(SSU::CardType card, int eventId, SSU::EventStatus status);
        };

        // Авторизация
        class SetLogin : public Request
        {
        public:
            SetLogin(const QString &login);
        };

        class SetPassword : public Request
        {
        public:
            SetPassword(const QString &password);
        };

        //------- Информация о пользователе -------
        class GetCurrentUserInfo : public Request
        {
        public:
            static QString cmd() { return "get userinfo"; }
            GetCurrentUserInfo();
        };

        //------- Считать информацию о конфигурации списка плат -------
        class GetPanelConfiguration : public Request
        {
        public:
            static QString cmd() { return "get cardsConfig"; }
            GetPanelConfiguration();
        };

        //------- Установить информацию о конфигурации списка плат -------
        class SetPanelConfiguration : public SetFileRequest
        {
        public:
            static QString cmd() { return "set cardsConfig"; }
            SetPanelConfiguration(const QString &path, const QString &fileName);
            Request *execute(bool success) override;
        };


        class SetDeviceDateTime : public Request
        {
        public:
            static QString cmd() { return "set datetime"; }
            SetDeviceDateTime();
            SetDeviceDateTime(const QDateTime &datetime);
            QString arg() const override;
        };



        //------- Управление пользователями -------

        //------- Считать список пользователей -------
        class GetUserList : public Request
        {
        public:
            static QString cmd() { return "get userslist"; }
            GetUserList();
        };

        //------- Добавить пользователя -------

        class AddUser : public Request
        {
        public:
            static QString cmd() { return "user add"; }
            AddUser(const QString &username, const QString &password, SSU::UserLevel level);
        };

        //------- Удалить пользователя -------
        class DeleteUser : public Request
        {
        public:
            static QString cmd() { return "user del"; }
            DeleteUser(const QString &username);
        };

        //------- Заблокировать пользователя -------
        class BlockUser : public Request
        {
        public:
            static QString cmd() { return "user block"; }
            BlockUser(const QString &username, const QDateTime &dt);
        };

        //------- Разблокировать пользователя -------

        class UnblockUser : public Request
        {
        public:
            static QString cmd() { return "user block"; }
            UnblockUser(const QString &username);      //@N переделать..
        };

        //------- Установить пароль пользователю -------
        class SetUserPassword : public Request
        {
        public:
            static QString cmd() { return "user setpass"; }
            SetUserPassword(const QString &username = "", const QString &password = "");
        };

        //------- Пороговые значения -------
        class GetThresholds : public Request
        {
        public:
            static QString cmd() { return "get threshold"; }
            GetThresholds();
        };

        class SetThreshold : public Request
        {
        public:
            static QString cmd() { return "set threshold"; }
            SetThreshold(int channel, SSU::MeasDataType type, int interval, int value);
        };

        class SetAllThreshols : public Request
        {
        public:
            static QString cmd() { return "set allchanthrs"; }
            SetAllThreshols(int channel);
        };

        class ResetThresholds : public Request
        {
        public:
            static QString cmd() { return "set resthr"; }
            ResetThresholds();
        };


        //------- Сбросить устройство -------
        class ResetDevice : public Request
        {
        public:
            static QString cmd() { return "set freset"; }
            ResetDevice();
        };

        //------- Сбросить измерения -------
        class ResetMeasurements : public Request
        {
        public:
            static QString cmd() { return "set measreset"; }
            // -1 сбросит у всех каналов
            // >=0 у переданного
            ResetMeasurements(int channel = -1);
        };

        //------- Сбросить журнал измерений -------
        class ResetMeasLog : public Request
        {
        public:
            static QString cmd() { return "set clearlog Meas"; }
            ResetMeasLog();
        };

        //------- Сбросить журнал действий пользователя -------

        class ResetUserLog : public Request
        {
        public:
            static QString cmd() { return "set clearlog User"; }
            ResetUserLog();
        };

        //------- Сбросить журнал событий -------
        class ResetEventLog : public Request
        {
        public:
            static QString cmd() { return "set clearlog Events"; }
            ResetEventLog();
        };

        //------- Журнал истории событий -------
        class GetEventLog : public Request
        {
        public:
            static QString cmd() { return "get logs"; }
            GetEventLog(Device *device);    // получать последние данные всегда. Смотреть последнию метку в базе данных и считывать начиная с нее
            GetEventLog(const QDateTime &from, const QDateTime &to = QDateTime(), Device *device = nullptr);

            Request *execute(bool success) override;
            SendResult send(TcpReader *reader) override;
            QString arg() const override;

        private:
            Device *device_;
        };

        //------- Журнал истории действий пользователя -------
        class GetUserLog : public Request
        {
        public:
            static QString cmd() { return "get ulgs"; }
            GetUserLog(Device *device);    // получать последние данные всегда. Смотреть последнию метку в базе данных и считывать начиная с нее
            GetUserLog(const QDateTime &from, const QDateTime &to = QDateTime(), Device *device = nullptr);

            Request *execute(bool success) override;
            SendResult send(TcpReader *reader) override;
            QString arg() const override;

        private:
            Device *device_;
        };

        //------- Считывания списка измеряемых сигналов -------
        class GetReclist : public Request
        {
        public:
            static QString cmd() { return "get reclist"; }
            GetReclist(Device *device);    // получать последние данные всегда. Смотреть последнию метку в базе данных и считывать начиная с нее
            GetReclist(const QDateTime &from, const QDateTime &to = QDateTime(), Device *device = nullptr);

            Request *execute(bool success) override;
            SendResult send(TcpReader *reader) override;
            QString arg() const override;

        private:
            Device *device_;
        };

        //------- Считывания историю измерений -------
        class GetMeasLog : public Request
        {
        public:
            static QString cmd() { return "get mlgs"; }
            GetMeasLog(int channel, Device *device);    // получать последние данные всегда. Смотреть последнию метку в базе данных и считывать начиная с нее
            GetMeasLog(int channel, const QDateTime &from, const QDateTime &to = QDateTime(), Device *device = nullptr);

            Request *execute(bool success) override;
            SendResult send(TcpReader *reader) override;
            QString arg() const override;

        private:
            Device *device_;
        };

        //------- Считать текущие измерения -------
        class GetCurrentMeas : public Request
        {
        public:
            static QString cmd() { return "get meas"; }
            GetCurrentMeas();
        };


        //------- Скачивание базы данных устройства -------
        class DownloadDb : public GetFileRequest {
        public:
            static QString cmd() { return "get ssuDB"; }
            DownloadDb(const QString &path, Device *device = nullptr);        // Device нужен, для того, чтобы давать реакцию в execute
            Request *execute(bool success) override;
        };

        //------- Скачивание конфигурации устройства -------
        class DownloadConfig : public GetFileRequest {
        public:
            static QString cmd() { return "get ssuConfig"; }
            DownloadConfig(const QString &path, Device *device = nullptr);        // Device нужен, для того, чтобы давать реакцию в execute
            Request *execute(bool success) override;
        };

        //------- Загрузка базы данных в устройство -------
        class UploadDb : public SetFileRequest {
        public:
            static QString cmd() { return "set ssuDB"; }
            UploadDb(const QString &path, Device *device = nullptr);        // Device нужен, для того, чтобы давать реакцию в execute
            Request *execute(bool success) override;
        };

        //------- Загрузка конфигурации в устройство -------
        class UploadConfig : public SetFileRequest {
        public:
            static QString cmd() { return "set ssuConfig"; }
            UploadConfig(const QString &path, Device *device = nullptr);        // Device нужен, для того, чтобы давать реакцию в execute
            Request *execute(bool success) override;
        };

        //------- Загрузка прошивки контроллеров -------
        class UploadFirmware : public SetFileRequest {
        public:
            static QString cmd() { return "set firmware"; }
            UploadFirmware(const QString &path, const QString &fileName, const QString &slot, Device *device = nullptr);
            UploadFirmware(const QString &path, const QString &fileName, SSU::CardType type, Device *device = nullptr);
            Request *execute(bool success) override;
        };

        double getMeasCoef(SSU::MeasDataType type);          // коэф при считывании значений измерений. То есть, что пришло * coef
        double getThresholdCoef(SSU::MeasDataType type);     // коэф при считывании значений измерений. То есть, что есть * coef

        Vch003::E1Bit e1bitFromString(const QStringRef &e1bit);
        QString e1bitToString(Vch003::E1Bit bit);

        SSU::SignalType signalTypeFromString(const QStringRef &type);
        QString signalTypeToString(SSU::SignalType type);

        QString cardTypeToString(SSU::CardType type);
        SSU::CardType cardTypeFromString(const QStringRef &type);
        SSU::CardType cardTypeFromIndex(int index);

        QString eventStatusToString(SSU::EventStatus status);
        SSU::EventStatus eventStatusFromString(const QStringRef &status);

        QString userLevelToString(SSU::UserLevel level);
        SSU::UserLevel userLevelFromString(const QStringRef &level);

        // StatusThreshold
        SSU::StatusThreshold statusThresholdFromString(const QStringRef &status);
        QString statusThresholdToString(SSU::StatusThreshold status);

        //SSM
        SSU::SSM ssmFromString(const QStringRef &ssm);
        QString ssmToString(SSU::SSM ssm);

        // Hemisphere
        SSU::Hemisphere hemisphereFromString(const QStringRef &hemisphere);


        LedState ledStateFromString(const QStringRef &string);


        int inputChannelState(int value);

        Vch003::Sensitivity sensitivityFromString(const QStringRef &e1bit);
        QString sensitivityToString(Vch003::Sensitivity sensitivity);

        Vch003::Sensitivity sensitivityFromRawData(int value, bool permission = true); // permission - нужно ли проверять разрешение. Не у всех он есть
        int sensitivityToRawData(Vch003::Sensitivity sensitivity, bool permission = true);

        SSU::MeasDataType measDataTypeFromIndex(int index);

        SSU::MeasDataType measDataTypeFromString(const QStringRef &type);
        QString measDataTypeToString(SSU::MeasDataType type);

        SSU::WorkingMode workingModeFromString(const QStringRef &mode);
        QString workingModeToString(SSU::WorkingMode mode, int manualChannel);

        SSU::PllState pllStateFromString(const QStringRef &state);

        bool activeCardFromString(const QStringRef &active);
        QString activeCardToString(bool active);

        QString satelliteSystemToString(SSU::SatelliteSystem system);
        SSU::SatelliteSystem satelliteSystemFromString(const QStringRef &sys);
        SSU::SatelliteSystem satelliteSystemFromIndex(int index);


        SSU::GeneratorType generatorTypeFromString(const QStringRef &type);

        SSU::SsuType ssuTypeFromString(const QStringRef &type);
        QString ssuTypeToString(SSU::SsuType type);


        Ptp::PtpMode ptpModeFromString(const QStringRef &value);
        QString ptpModeToString(Ptp::PtpMode value);

        Ptp::TxMode txModeFromString(const QStringRef &value);
        QString txModeToString(Ptp::TxMode value);

        Ptp::WayMode wayModeFromString(const QStringRef &value);
        QString wayModeToString(Ptp::WayMode value);

        Ptp::SyncMode syncModeFromString(const QStringRef &value);
        QString syncModeToString(Ptp::SyncMode value);

        Ptp::DelayMechanism delayMechanismFromString(const QStringRef &value);
        QString delayMechanismToString(Ptp::DelayMechanism value);

        Ptp::Protocol protocolFromString(const QStringRef &value);
        QString protocolToString(Ptp::Protocol value);

        EthernetPort::PpsSource ppsSourceFromString(const QStringRef &value);
        QString ppsSourceToString(EthernetPort::PpsSource value);

        EthernetPort::Speed portSpeedFromString(const QStringRef &value);
        QString portSpeedToString(EthernetPort::Speed value);
    }
}




#endif // VCH003REQUESTS_H
