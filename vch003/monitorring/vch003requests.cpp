#include "vch003requests.h"
#include <QDir>
#include "vch003/vch003device.h"
#include "vch003/database/vch003tables.h"
#include "global.h"
#include "csettings.h"

namespace ssu {
    //------- Виртуальный класс считывания логов -------
    namespace vch003request {
        QString dateIntervalToString(const QDateTime &from, const QDateTime &to)
        {
            QString interval;
            if(from.isValid()) interval.append(from.toUTC().toString(DEVICE_TIME_FORMAT));
            if(to.isValid() && !interval.isEmpty()) {
                interval.append(" - ");
                interval.append(to.toUTC().toString(DEVICE_TIME_FORMAT));
            }
            return interval;
        }

        QDateTime lastLogUpdateTime(Device *device, int table)
        {
            auto lastDay = QDateTime::currentDateTimeUtc().addSecs(-SSU::kMaxNumberOfSecToUpdateLogData);
            if(device == nullptr)
                return lastDay;
            // считаем лог начиная с последнего момента или за последние сутки..
            //            auto lastSec = device->databaseExecutor()->lastValue(table, &isok);   //@N Не знаю как лучше сделать. Раньше делал запрос в базу в запросе. Но тк все это из другого потока, то пока сделаю так
            auto lastSec = static_cast<Vch003Device*>(device)->lastDownloadTime(table);     //@N мб стоит добавлять 1 сек?
            return std::max(QDateTime::fromSecsSinceEpoch(lastSec, Qt::UTC),
                            lastDay);
        }

        //_________________________________
        GetInventory::GetInventory() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        GetCardParameters::GetCardParameters(const QString &slot) : Request(cmd(), "", slot) {}

        // ------------------------------------------------------------------------

        GetSatelliteList::GetSatelliteList(const QString &slot) : Request(cmd(), "", slot) {}

        // ------------------------------------------------------------------------

        GetGnssPortSettings::GetGnssPortSettings(const QString &slot, int portIndex) : Request(cmd() + QString::number(portIndex), "", slot) {}

        // ------------------------------------------------------------------------
        GetGnssPortStatistics::GetGnssPortStatistics(const QString &slot, int portIndex) : Request(cmd() + QString::number(portIndex), "", slot) {}

        // ------------------------------------------------------------------------
        GetGnssNetworkConfiguration::GetGnssNetworkConfiguration(const QString &slot, int portIndex, int confIndex) : Request(cmd() + QString::number(portIndex),
                                                                                                                              QString::number(confIndex), slot) {}



        // ------------------------------------------------------------------------

        ClearGnssPortConfiguration::ClearGnssPortConfiguration(const QString &slot, int portIndex, int confIndex) : Request(cmd() + QString::number(portIndex),
                                                                                                                            QString::number(confIndex), slot) {}
        // ------------------------------------------------------------------------
        ClearNtpConfiguration::ClearNtpConfiguration(const QString &slot, int portIndex, int confIndex) : Request(cmd() + QString::number(portIndex),
                                                                                                                  QString::number(confIndex), slot) {}

        // ------------------------------------------------------------------------
        ClearGnssPortStatistics::ClearGnssPortStatistics(const QString &slot, int portIndex) : Request(cmd() + QString::number(portIndex), "", slot) {}

        // ------------------------------------------------------------------------
        ClearPtpConfiguration::ClearPtpConfiguration(const QString &slot, int portIndex, int confIndex) : Request(cmd() + QString::number(portIndex),
                                                                                                                  QString::number(confIndex), slot) {}

        // ------------------------------------------------------------------------

        SetCardParameter::SetCardParameter(const QString &slot, const QString &parameter, const QString &value)
            : Request("set " + parameter, value, slot) {}

        SetCardParameter::SetCardParameter(const QString &slot, const QString &parameter, const QStringList &values)
            : Request("set " + parameter, values.join(","), slot) {}

        // ------------------------------------------------------------------------

        ResetCard::ResetCard(const QString &slot) : Request(cmd(), "", slot) {}

        // ------------------------------------------------------------------------

        ResetToDefaultCard::ResetToDefaultCard(const QString &slot) : Request(cmd(), "", slot) {}

        // ------------------------------------------------------------------------

        GetActiveEvents::GetActiveEvents() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        GetEventDescription::GetEventDescription() :  Request(cmd()) {}

        GetEventDescription::GetEventDescription(SSU::CardType card) :
            Request(cmd(), cardTypeToString(card)) {}

        // ------------------------------------------------------------------------

        SetCardEventStatus::SetCardEventStatus(SSU::CardType card, int eventId, SSU::EventStatus status) :
            Request(cmd(), cardTypeToString(card) + " " + QString::number(eventId) + " " + eventStatusToString(status)) {}

        // ------------------------------------------------------------------------

        SetLogin::SetLogin(const QString &login) : Request(login) {}

        // ------------------------------------------------------------------------

        SetPassword::SetPassword(const QString &password) : Request(password) {}

        // ------------------------------------------------------------------------

        GetCurrentUserInfo::GetCurrentUserInfo() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        GetPanelConfiguration::GetPanelConfiguration() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        SetPanelConfiguration::SetPanelConfiguration(const QString &path, const QString &fileName) : SetFileRequest(path, fileName, cmd()) {}
        Request *SetPanelConfiguration::execute(bool success)
        {
            QFile::remove(absoluteFilePath());
            return Request::execute(success);
        }

        // ------------------------------------------------------------------------

        GetUserList::GetUserList() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        SetDeviceDateTime::SetDeviceDateTime() : Request(cmd()) {}

        SetDeviceDateTime::SetDeviceDateTime(const QDateTime &datetime):
            Request(cmd(), datetime.toUTC().toString("yyyy.MM.dd-HH:mm:ss")) {}

        QString SetDeviceDateTime::arg() const
        {
            if(arg_.isEmpty())
                return QDateTime::currentDateTimeUtc().toString("yyyy.MM.dd-HH:mm:ss");
            return Request::arg();
        }

        // ------------------------------------------------------------------------

        DownloadDb::DownloadDb(const QString &path, Device *device) :
            GetFileRequest(path, DEVICE_DATABASE_FILE_NAME, cmd(), "", "", device)
        {

        }

        Request *DownloadDb::execute(bool success)
        {
            if(success) {
                const QString tempFile = SSU::downloadTempFile();
                if(QFile::exists(tempFile)) {
                    // создадим папку бекапа, если ее нет
                    QDir dir(path_);
                    if(!dir.exists()) dir.mkpath(path_);
                    // Похоже все хорошо. Нужно переименовать файл и переместить из временных
                    QFile::rename(tempFile, absoluteFilePath());
                }
                else {
                    qWarning() << "DownloadDb error!!";
                }
            }

            return GetFileRequest::execute(success);
        }

        // ------------------------------------------------------------------------

        DownloadConfig::DownloadConfig(const QString &path, Device *device) :
            GetFileRequest(path, DEVICE_CARD_CONFIG_FILE_NAME, cmd(), "", "", device)
        {

        }

        Request *DownloadConfig::execute(bool success)
        {
            if(success) {
                const QString tempFile = SSU::downloadTempFile();
                if(QFile::exists(tempFile)) {
                    // создадим папку бекапа, если ее нет
                    QDir dir(path_);
                    if(!dir.exists()) dir.mkpath(path_);

                    // Похоже все хорошо. Нужно переименовать файл и переместить из временных
                    QFile::rename(tempFile, absoluteFilePath());

                    // Нужно отправить команду на скачивания базы данных
                    return new DownloadDb(path_, device_);
                }
                else {
                    qWarning() << "DownloadConfig error!!";
                }
            }

            return GetFileRequest::execute(success);
        }

        // ------------------------------------------------------------------------

        UploadDb::UploadDb(const QString &path, Device *device) :
            SetFileRequest(path, DEVICE_DATABASE_FILE_NAME, cmd(), "", "", device)
        {

        }

        Request *UploadDb::execute(bool success)
        {
            if(success) {

            }
            else {
                // error!
                qWarning() << "Upload db error!!";
            }

            return SetFileRequest::execute(success);
        }

        // ------------------------------------------------------------------------

        UploadConfig::UploadConfig(const QString &path, Device *device) :
            SetFileRequest(path, DEVICE_CARD_CONFIG_FILE_NAME, cmd(), "", "", device)
        {

        }

        Request *UploadConfig::execute(bool success)
        {
            if(success) {
                // Нужно отправить команду на отправку базы данных
                return new UploadDb(path_, device_);
            }
            else {
                // error!
                qWarning() << "Upload config error!!";
            }

            return SetFileRequest::execute(success);
        }

        // ------------------------------------------------------------------------

        UploadFirmware::UploadFirmware(const QString &path, const QString &fileName, const QString &slot, Device *device) :
            SetFileRequest(path, fileName, cmd(), "", slot, device)
        {

        }

        UploadFirmware::UploadFirmware(const QString &path, const QString &fileName, SSU::CardType type, Device *device) :
            SetFileRequest(path, fileName, cmd(), cardTypeToString(type), "", device)
        {

        }

        Request *UploadFirmware::execute(bool success)
        {
            if(success) {

            }
            else {
                // error!
                qWarning() << "UploadFirmware error!!";
            }

            return SetFileRequest::execute(success);
        }

        // ------------------------------------------------------------------------


        AddUser::AddUser(const QString &username, const QString &password, SSU::UserLevel level) :
            Request(cmd(), username + " " + password + " " + userLevelToString(level)) {}

        // ------------------------------------------------------------------------

        DeleteUser::DeleteUser(const QString &username) : Request(cmd(), username) {}

        // ------------------------------------------------------------------------

        BlockUser::BlockUser(const QString &username, const QDateTime &dt) : Request(cmd(), username + " " + dt.toUTC().toString(DEVICE_TIME_FORMAT)) {}

        // ------------------------------------------------------------------------

        UnblockUser::UnblockUser(const QString &username) : Request(cmd(), username + " " + QDateTime::currentDateTimeUtc().addDays(-100).toString(DEVICE_TIME_FORMAT)) {}

        // ------------------------------------------------------------------------

        SetUserPassword::SetUserPassword(const QString &username, const QString &password) : Request(cmd(), username + " " + password) {}



        // ------------------------------------------------------------------------
        ResetDevice::ResetDevice() : Request(cmd()) {}
        ResetMeasurements::ResetMeasurements(int channel) : Request(cmd(), channel == -1 ? "" : QString::number(channel)) {}
        ResetMeasLog::ResetMeasLog() : Request(cmd()) {}
        ResetUserLog::ResetUserLog() : Request(cmd()) {}
        ResetEventLog::ResetEventLog() : Request(cmd()) {}

        // ------------------------------------------------------------------------


        GetThresholds::GetThresholds() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        SetAllThreshols::SetAllThreshols(int channel) : Request(cmd(), QString::number(channel)) {}

        // ------------------------------------------------------------------------

        SetThreshold::SetThreshold(int channel, SSU::MeasDataType type, int interval, int value)
            : Request(cmd(), QString::number(channel) + " " +
                      measDataTypeToString(type) + " " +
                      QString::number(interval) + " " +
                      QString::number(value)) {}

        // ------------------------------------------------------------------------

        ResetThresholds::ResetThresholds() : Request(cmd()) {}

        // ------------------------------------------------------------------------

        GetEventLog::GetEventLog(Device *device) : Request(cmd()),
            device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        GetEventLog::GetEventLog(const QDateTime &from, const QDateTime &to, Device *device) :
            Request(cmd(), dateIntervalToString(from, to)),
            device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        Request *GetEventLog::execute(bool success)
        {
            Q_UNUSED(success)
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setEventLogLoading(false);
            return Request::execute(success);
        }

        Request::SendResult GetEventLog::send(TcpReader *reader)
        {
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setEventLogLoading(true);
            return std::move(Request::send(reader));
        }

        QString GetEventLog::arg() const
        {
            if(arg_.isEmpty() && device_ != nullptr)
                return lastLogUpdateTime(device_, static_cast<int>(Vch003::DatabaseTable::EventLog)).toUTC().toString(DEVICE_TIME_FORMAT);
            return Request::arg();
        }

        // ------------------------------------------------------------------------

        GetUserLog::GetUserLog(Device *device) : Request(cmd()),
            device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        GetUserLog::GetUserLog(const QDateTime &from, const QDateTime &to, Device *device) :
            Request(cmd(), dateIntervalToString(from, to)),
            device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        Request *GetUserLog::execute(bool success)
        {
            Q_UNUSED(success)
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setUserActivityLogLoading(false);
            return Request::execute(success);
        }

        Request::SendResult GetUserLog::send(TcpReader *reader)
        {
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setUserActivityLogLoading(true);
            return std::move(Request::send(reader));
        }

        QString GetUserLog::arg() const
        {
            if(arg_.isEmpty() && device_ != nullptr) {
                return lastLogUpdateTime(device_, static_cast<int>(Vch003::DatabaseTable::UserActivityLog)).toUTC().toString(DEVICE_TIME_FORMAT);
            }
            return Request::arg();
        }

        // ------------------------------------------------------------------------

        GetReclist::GetReclist(Device *device) : Request(cmd()),
            device_(device)
        {
            setResponseTime(SSU::kResponseTime * 2);
        }

        GetReclist::GetReclist(const QDateTime &from, const QDateTime &to, Device *device) :
            Request(cmd(), dateIntervalToString(from, to)),
            device_(device)
        {
            setResponseTime(SSU::kResponseTime * 2);
        }

        Request *GetReclist::execute(bool success)
        {
            Q_UNUSED(success)
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setMeasLogLoading(false);
            return Request::execute(success);
        }

        Request::SendResult GetReclist::send(TcpReader *reader)
        {
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setMeasLogLoading(true);
            return std::move(Request::send(reader));
        }

        QString GetReclist::arg() const
        {
            if(arg_.isEmpty() && device_ != nullptr)
                return lastLogUpdateTime(device_, static_cast<int>(Vch003::DatabaseTable::MeasLog)).toUTC().toString(DEVICE_TIME_FORMAT);
            return Request::arg();
        }

        // ------------------------------------------------------------------------

        GetMeasLog::GetMeasLog(int channel, Device *device) : Request(cmd() + " " + QString::number(channel)),
            device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        GetMeasLog::GetMeasLog(int channel, const QDateTime &from, const QDateTime &to, Device *device)
            : Request(cmd() + " " + QString::number(channel), dateIntervalToString(from, to)),
              device_(device)
        {
            setResponseTime(SSU::kLogReadResponseTime);
        }

        Request *GetMeasLog::execute(bool success)
        {
            Q_UNUSED(success)
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setMeasLogLoading(false);
            return Request::execute(success);
        }

        Request::SendResult GetMeasLog::send(TcpReader *reader)
        {
            if(device_ != nullptr)
                static_cast<Vch003Device*>(device_)->setMeasLogLoading(true);
            return std::move(Request::send(reader));
        }

        QString GetMeasLog::arg() const
        {
            if(arg_.isEmpty() && device_ != nullptr) {
                //@N тут нужно считывать начиная с последнего значения для этого канала..
                return lastLogUpdateTime(device_, static_cast<int>(Vch003::DatabaseTable::MeasLog)).toUTC().toString(DEVICE_TIME_FORMAT);
            }
            return Request::arg();
        }


        // ------------------------------------------------------------------------

        GetCurrentMeas::GetCurrentMeas() : Request(cmd()) {
            setResponseTime(SSU::kResponseTime * 2);
        }


        // ------------------------------------------------------------------------
        // ------------------------------------------------------------------------
        // ------------------------------------------------------------------------
        // ------------------------------------------------------------------------


        QString cardTypeToString(SSU::CardType type)
        {
            switch (type) {
                case SSU::Input:        return "CardInput";
                case SSU::Output:       return "CardOutput";
                case SSU::Generator:    return "CardClock";
                case SSU::Monitoring:   return "CardBM";
                case SSU::Distribution: return "CardDistrib";
                case SSU::Gnss:         return "CardGNSS";
                default: break;
            }
            return "";
        }

        SSU::CardType cardTypeFromString(const QStringRef &type)
        {
            if(type.isEmpty()) return SSU::InvalidType;
            if(type.contains("CardInput", Qt::CaseInsensitive))     return SSU::Input;
            if(type.contains("CardOutput", Qt::CaseInsensitive))    return SSU::Output;
            if(type.contains("CardClock", Qt::CaseInsensitive))     return SSU::Generator;
            if(type.contains("CardBM", Qt::CaseInsensitive))        return SSU::Monitoring;
            if(type.contains("CardDistrib", Qt::CaseInsensitive))   return SSU::Distribution;
            if(type.contains("CardGNSS", Qt::CaseInsensitive))      return SSU::Gnss;
            if(type.contains("Stub", Qt::CaseInsensitive))          return SSU::Stub;
            return SSU::InvalidType;
        }

        QString eventStatusToString(SSU::EventStatus status)
        {
            switch (status) {
                case SSU::EventStatus::Critical:    return "CRITICAL";
                case SSU::EventStatus::Minor:       return "MINOR";
                case SSU::EventStatus::Major:       return "MAJOR";
                case SSU::EventStatus::Warning:     return "NOT_ALARM";  //@N NOT_ALARM
                default: break;
            }
            return "";
        }

        SSU::EventStatus eventStatusFromString(const QStringRef &status)
        {
            if(status.isEmpty()) return SSU::EventStatus::Warning;
            if(status.contains("CRITICAL", Qt::CaseInsensitive)) return SSU::EventStatus::Critical;
            if(status.contains("MAJOR", Qt::CaseInsensitive)) return SSU::EventStatus::Major;
            if(status.contains("MINOR", Qt::CaseInsensitive)) return SSU::EventStatus::Minor;
            return SSU::EventStatus::Warning;  // NOT_ALARM
        }

        QString userLevelToString(SSU::UserLevel level)
        {
            switch (level) {
                case SSU::UserLevel::User: return "User";
                case SSU::UserLevel::Admin: return "Administrator";
                case SSU::UserLevel::Security: return "Security";
                default: return "";
            }
        }

        SSU::UserLevel userLevelFromString(const QStringRef &level)
        {
            if(level.isEmpty()) return SSU::UserLevel::User;
            if(level.contains("Administrator", Qt::CaseInsensitive)) return SSU::UserLevel::Admin;
            if(level.contains("Security", Qt::CaseInsensitive)) return SSU::UserLevel::Security;
            return SSU::UserLevel::User;   // "user"
        }

        SSU::SignalType signalTypeFromString(const QStringRef &type)
        {
            if(type.isEmpty()) return SSU::SignalType::Off;
            if(type.contains("1Mhz", Qt::CaseInsensitive)) return SSU::SignalType::MHz1;
            if(type.contains("5Mhz", Qt::CaseInsensitive)) return SSU::SignalType::MHz5;
            if(type.contains("10Mhz", Qt::CaseInsensitive)) return SSU::SignalType::MHz10;
            if(type.contains("2048Khz", Qt::CaseInsensitive)) return SSU::SignalType::KHz2048;
            if(type.contains("E1", Qt::CaseInsensitive)) return SSU::SignalType::E1;
            if(type.contains("Auto", Qt::CaseInsensitive)) return SSU::SignalType::Auto;
            return SSU::SignalType::Off;  // "OFF"
        }

        QString signalTypeToString(SSU::SignalType type)
        {
            switch (type) {
                case SSU::SignalType::Off: return "OFF";
                case SSU::SignalType::MHz1: return "1Mhz";
                case SSU::SignalType::MHz5: return "5Mhz";
                case SSU::SignalType::MHz10: return "10Mhz";
                case SSU::SignalType::KHz2048: return "2048Khz";
                case SSU::SignalType::E1: return "E1";
                case SSU::SignalType::Auto: return "Auto";
                default: return "";
            }
        }

        SSU::StatusThreshold statusThresholdFromString(const QStringRef &status)
        {
            if(status.isEmpty()) return SSU::StatusThreshold::None;
            if(status.contains("LOW", Qt::CaseInsensitive)) return SSU::StatusThreshold::Low;
            if(status.contains("HIGH", Qt::CaseInsensitive)) return SSU::StatusThreshold::High;
            return SSU::StatusThreshold::None;    // "NONE"
        }

        QString statusThresholdToString(SSU::StatusThreshold status)
        {
            switch (status) {
                case SSU::StatusThreshold::None: return "NONE";
                case SSU::StatusThreshold::Low: return "LOW";
                case SSU::StatusThreshold::High: return "HIGH";
                default: return "";
            }
        }

        SSU::SSM ssmFromString(const QStringRef &ssm)
        {
            if(ssm.isEmpty()) return SSU::SSM::None;
            if(ssm.contains("PRC", Qt::CaseInsensitive)) return SSU::SSM::PRC;
            if(ssm.contains("SSU_T", Qt::CaseInsensitive)) return SSU::SSM::SSU_T;
            if(ssm.contains("SSU_L", Qt::CaseInsensitive)) return SSU::SSM::SSU_L;
            if(ssm.contains("SEC", Qt::CaseInsensitive)) return SSU::SSM::SEC;
            if(ssm.contains("DNU", Qt::CaseInsensitive)) return SSU::SSM::DNU;
            return SSU::SSM::None;    // "NONE"
        }

        QString ssmToString(SSU::SSM ssm)
        {
            switch (ssm) {
                case SSU::SSM::None: return "NONE";
                case SSU::SSM::PRC: return "PRC";
                case SSU::SSM::SSU_T: return "SSU_T";
                case SSU::SSM::SSU_L: return "SSU_L";
                case SSU::SSM::SEC: return "SEC";
                case SSU::SSM::DNU: return "DNU";
                default: return "";
            }
        }

        LedState ledStateFromString(const QStringRef &string)
        {
            LedState state;

            state.blink = false;
            state.power = true;
            state.blinkInterval = 1000;

            if(string.compare(QString("GREEN"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::darkGreen;
            }
            else if(string.compare(QString("RED"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::red;
            }
            else if(string.compare(QString("YELLOW"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::yellow;
            }
            else if(string.compare(QString("GREEN_BLINK"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::darkGreen;
                state.blink = true;
            }
            else if(string.compare(QString("RED_BLINK"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::red;
                state.blink = true;
            }
            else if(string.compare(QString("YELLOW_BLINK"), Qt::CaseInsensitive) == 0) {
                state.color = Qt::yellow;
                state.blink = true;
            }
            else
                state.power = false;
            return state;
        }

        int inputChannelState(int value)
        {
            int state = 0;
            //    if((value & 7) == 0) state |= Vch003::InputSignalState::LossOfSignal;
            if(value & 8) state |= Vch003::InputSignalState::LossOfSignal;
            if(value & 16) state |= Vch003::InputSignalState::StatusDisqualification;

            int e1Error = value >> 8;
            if((e1Error & 1) == 0) state |= Vch003::InputSignalState::FailureOnTheTransmittingEnd;
            if((e1Error & 2) == 0) state |= Vch003::InputSignalState::AbsenceCRC;
            if((e1Error & 4) == 0) state |= Vch003::InputSignalState::LossOfMultiFrame;
            if((e1Error & 8) == 0) state |= Vch003::InputSignalState::LossOfFrame;
            if((e1Error & 16) == 0) state |= Vch003::InputSignalState::HDB3Error;
            if((e1Error & 32) == 0) state |= Vch003::InputSignalState::NoFrequencyCapture;

            return state;
        }

        Vch003::E1Bit e1bitFromString(const QStringRef &e1bit)
        {
            if(e1bit.isEmpty()) return Vch003::E1Bit::Sa4;
            if(e1bit.contains("sa4", Qt::CaseInsensitive)) return Vch003::E1Bit::Sa4;
            if(e1bit.contains("sa5", Qt::CaseInsensitive)) return Vch003::E1Bit::Sa5;
            if(e1bit.contains("sa6", Qt::CaseInsensitive)) return Vch003::E1Bit::Sa6;
            if(e1bit.contains("sa7", Qt::CaseInsensitive)) return Vch003::E1Bit::Sa7;
            if(e1bit.contains("sa8", Qt::CaseInsensitive)) return Vch003::E1Bit::Sa8;
            return Vch003::E1Bit::Sa4;
        }

        QString e1bitToString(Vch003::E1Bit bit)
        {
            switch (bit) {
                case Vch003::E1Bit::Sa4: return "sa4";
                case Vch003::E1Bit::Sa5: return "sa5";
                case Vch003::E1Bit::Sa6: return "sa6";
                case Vch003::E1Bit::Sa7: return "sa7";
                case Vch003::E1Bit::Sa8: return "sa8";
                default: return "";
            }
        }

        Vch003::Sensitivity sensitivityFromString(const QStringRef &e1bit)
        {
            if(e1bit.compare(QString("OFF"), Qt::CaseInsensitive) == 0)
                return Vch003::Sensitivity::Off;
            bool isok = false;
            int value = e1bit.toInt(&isok);
            return isok ? static_cast<Vch003::Sensitivity>(value)
                        : Vch003::Sensitivity::Off;
        }

        QString sensitivityToString(Vch003::Sensitivity sensitivity)
        {
            if(sensitivity == Vch003::Sensitivity::Off)
                return "OFF";
            return QString::number(static_cast<int>(sensitivity));
        }

        Vch003::Sensitivity sensitivityFromRawData(int value, bool permission)
        {
            if(permission && ((value & 0x4) == 0))
                return Vch003::Sensitivity::Off;
            //    return static_cast<Vch003::Sensitivity>(((value & 0x2) >> 1) | ((value & 0x1) << 1));
            return static_cast<Vch003::Sensitivity>(value & 0x3);
        }

        int sensitivityToRawData(Vch003::Sensitivity sensitivity, bool permission)
        {
            if(sensitivity == Vch003::Sensitivity::Off)
                return 0x0;
            //    int sens = static_cast<int>(sensitivity);
            //    return (((sens & 0x2) >> 1) | ((sens & 0x1) << 1)) | (permission ? 0x4 : 0x0);
            return static_cast<int>(sensitivity) | (permission ? 0x4 : 0x0);
        }


        SSU::MeasDataType measDataTypeFromString(const QStringRef &type)
        {
            if(type.isEmpty()) return SSU::MeasDataType::Invalid;
            if(type.contains("MTIE", Qt::CaseInsensitive)) return SSU::MeasDataType::Mtie;
            if(type.contains("TDEV", Qt::CaseInsensitive)) return SSU::MeasDataType::Tdev;
            if(type.contains("df_F", Qt::CaseInsensitive)) return SSU::MeasDataType::Rf;
            return SSU::MeasDataType::Invalid;
        }

        QString measDataTypeToString(SSU::MeasDataType type)
        {
            switch (type) {
                case SSU::MeasDataType::Mtie: return "MTIE";
                case SSU::MeasDataType::Tdev: return "TDEV";
                case SSU::MeasDataType::Rf: return "Df_F";
                default: return "";
            }
        }

        double getMeasCoef(SSU::MeasDataType type)
        {
            if(CSettings::getInstance()->displayingValuesWithoutCoefficients())
                return 1; //@N getMeasCoef debug!!!!

            switch (type) {
                case SSU::MeasDataType::Mtie: return 1.e-9;
                case SSU::MeasDataType::Tdev: return 1.e-9;
                case SSU::MeasDataType::Rf:   return 1.e-12;
                default: break;
            }
            return 1.e-9;
        }

        double getThresholdCoef(SSU::MeasDataType type)
        {
            if(CSettings::getInstance()->displayingValuesWithoutCoefficients())
                return 1; //@N getMeasCoef debug!!!!

            switch (type) {
                case SSU::MeasDataType::Mtie: return 1.e-9;
                case SSU::MeasDataType::Tdev: return 1.e-12;
                case SSU::MeasDataType::Rf:   return 1.e-12;
                default: break;
            }
            return 1.e-9;
        }

        SSU::WorkingMode workingModeFromString(const QStringRef &mode)
        {
            if(mode.isEmpty()) return SSU::WorkingMode::AutoMode;
            if(mode.contains("MANUAL", Qt::CaseInsensitive)) return SSU::WorkingMode::ManualMode;
            if(mode.contains("FORCE_HOLDOVER", Qt::CaseInsensitive)) return SSU::WorkingMode::ForcedHoldover;
            return SSU::WorkingMode::AutoMode;    // "AUTO"
        }

        QString workingModeToString(SSU::WorkingMode mode, int manualChannel)
        {
            switch (mode) {
                case SSU::WorkingMode::AutoMode: return "AUTO";
                case SSU::WorkingMode::ManualMode: return "MANUAL " + QString::number(manualChannel);
                case SSU::WorkingMode::ForcedHoldover: return "FORCE_HOLDOVER";
                default: return "";
            }
        }

        SSU::PllState pllStateFromString(const QStringRef &state) {
            if(state.isEmpty()) return SSU::PllState::Error;
            if(state.contains("Tracking", Qt::CaseInsensitive))     return SSU::PllState::Tracking;
            if(state.contains("Warmup", Qt::CaseInsensitive))       return SSU::PllState::Warmup;
            if(state.contains("Freerun", Qt::CaseInsensitive))      return SSU::PllState::Freerun;
            if(state.contains("Holdover", Qt::CaseInsensitive))     return SSU::PllState::Holdover;
            if(state.contains("Adjustment", Qt::CaseInsensitive))   return SSU::PllState::Adjustment;
            if(state.contains("NoHold", Qt::CaseInsensitive))       return SSU::PllState::NoCapture;
            if(state.contains("Error", Qt::CaseInsensitive))        return SSU::PllState::Error;
            return SSU::PllState::Error;    // "Error"
        }

        bool activeCardFromString(const QStringRef &active)
        {
            if(active.isEmpty())
                return false;
            if(active.contains("Active", Qt::CaseInsensitive))
                return true;
            return false;
        }

        QString activeCardToString(bool active)
        {
            return active ? "Active"
                          : "Passive";
        }

        SSU::CardType cardTypeFromIndex(int index)
        {
            switch (index) {
                case 1: return SSU::Input;
                case 3: return SSU::Output;
                case 4: return SSU::Generator;
                case 5: return SSU::Monitoring;
                case 6: return SSU::Stub;
                case 7: return SSU::Gnss;
                case 8: return SSU::Distribution;
                default: break;
            }
            return SSU::CardType::InvalidType;
        }

        SSU::Hemisphere hemisphereFromString(const QStringRef &hemisphere)
        {
            if(hemisphere.contains("N", Qt::CaseInsensitive)) return SSU::Hemisphere::North;
            if(hemisphere.contains("W", Qt::CaseInsensitive)) return SSU::Hemisphere::West;
            if(hemisphere.contains("S", Qt::CaseInsensitive)) return SSU::Hemisphere::South;
            if(hemisphere.contains("E", Qt::CaseInsensitive)) return SSU::Hemisphere::East;
            return SSU::Hemisphere::East;
        }

        SSU::MeasDataType measDataTypeFromIndex(int index)
        {
            switch (index) {
                case 0: return SSU::MeasDataType::Tdev;
                case 1: return SSU::MeasDataType::Mtie;
                case 2: return SSU::MeasDataType::Rf;
                default: break;
            }
            return SSU::MeasDataType::Invalid;
        }

        QString satelliteSystemToString(SSU::SatelliteSystem system)
        {
            switch (system) {
                case SSU::SatelliteSystem::GPS:     return "GPS";
                case SSU::SatelliteSystem::GLONASS: return "Glonass";
                case SSU::SatelliteSystem::Galileo: return "Galileo";
                case SSU::SatelliteSystem::BeiDou:  return "BeiDou";
                case SSU::SatelliteSystem::QZSS:    return "QZSS";
                case SSU::SatelliteSystem::IRNSS:   return "IRNSS";
                default: return "";
            }
        }

        SSU::SatelliteSystem satelliteSystemFromString(const QStringRef &sys)
        {
            if(sys.isEmpty())
                return SSU::SatelliteSystem::UnknownSystem;
            if(sys.contains("GPS", Qt::CaseInsensitive))        return SSU::SatelliteSystem::GPS;
            if(sys.contains("Glonass", Qt::CaseInsensitive))    return SSU::SatelliteSystem::GLONASS;
            if(sys.contains("Galileo", Qt::CaseInsensitive))    return SSU::SatelliteSystem::Galileo;
            if(sys.contains("BeiDou", Qt::CaseInsensitive))     return SSU::SatelliteSystem::BeiDou;
            if(sys.contains("QZSS", Qt::CaseInsensitive))       return SSU::SatelliteSystem::QZSS;
            if(sys.contains("IRNSS", Qt::CaseInsensitive))      return SSU::SatelliteSystem::IRNSS;
            return SSU::SatelliteSystem::UnknownSystem;
        }

        SSU::SatelliteSystem satelliteSystemFromIndex(int index)
        {
            switch (index) {
                case 0: return SSU::SatelliteSystem::GPS;
                case 1: return SSU::SatelliteSystem::GLONASS;
                case 2: return SSU::SatelliteSystem::BeiDou;
                case 3: return SSU::SatelliteSystem::Galileo;
                default: break;
            }
            return SSU::SatelliteSystem::UnknownSystem;
        }

        SSU::GeneratorType generatorTypeFromString(const QStringRef &type)
        {
            if(type.contains("rubidium", Qt::CaseInsensitive)) return SSU::GeneratorType::Rubidium;
            if(type.contains("quartz", Qt::CaseInsensitive)) return SSU::GeneratorType::Quartz;
            return SSU::GeneratorType::Quartz;
        }

        SSU::SsuType ssuTypeFromString(const QStringRef &type)
        {
            if(type.contains("SSU_A", Qt::CaseInsensitive)) return SSU::SsuType::SSU_A;
            if(type.contains("SSU_B", Qt::CaseInsensitive)) return SSU::SsuType::SSU_B;
            return SSU::SsuType::SSU_A;
        }

        QString ssuTypeToString(SSU::SsuType type)
        {
            switch (type) {
                case SSU::SsuType::SSU_A:
                    return "SSU_A";
                case SSU::SsuType::SSU_B:
                    return "SSU_B";
                default:
                    break;
            }
            return "";
        }

        Ptp::PtpMode ptpModeFromString(const QStringRef &value)
        {
            if(value.contains("Master", Qt::CaseInsensitive)) return Ptp::PtpMode::Master;
            if(value.contains("Slave", Qt::CaseInsensitive)) return Ptp::PtpMode::Slave;
            return Ptp::PtpMode::Invalid;
        }

        QString ptpModeToString(Ptp::PtpMode value)
        {
            switch (value) {
                case Ptp::PtpMode::Master:  return "Master";
                case Ptp::PtpMode::Slave:   return "Slave";
                default:
                    return "";
            }
        }

        Ptp::TxMode txModeFromString(const QStringRef &value)
        {
            if(value.contains("Master", Qt::CaseInsensitive))   return Ptp::TxMode::Mixed;
            if(value.contains("Slave", Qt::CaseInsensitive))    return Ptp::TxMode::Multicast;
            if(value.contains("Slave", Qt::CaseInsensitive))    return Ptp::TxMode::Unicast;
            return Ptp::TxMode::Invalid;
        }

        QString txModeToString(Ptp::TxMode value)
        {
            switch (value) {
                case Ptp::TxMode::Mixed:        return "Mixed";
                case Ptp::TxMode::Multicast:    return "Multicast";
                case Ptp::TxMode::Unicast:      return "Unicast";
                default:
                    return "";
            }
        }

        Ptp::WayMode wayModeFromString(const QStringRef &value)
        {
            if(value.contains("OneWay", Qt::CaseInsensitive)) return Ptp::WayMode::OneWay;
            if(value.contains("TwoWay", Qt::CaseInsensitive)) return Ptp::WayMode::TwoWay;
            return Ptp::WayMode::Invalid;
        }

        QString wayModeToString(Ptp::WayMode value)
        {
            switch (value) {
                case Ptp::WayMode::OneWay:   return "OneWay";
                case Ptp::WayMode::TwoWay:   return "TwoWay";
                default:
                    return "";
            }
        }

        Ptp::SyncMode syncModeFromString(const QStringRef &value)
        {
            if(value.contains("OneStep", Qt::CaseInsensitive)) return Ptp::SyncMode::OneStep;
            if(value.contains("TwoStep", Qt::CaseInsensitive)) return Ptp::SyncMode::TwoStep;
            return Ptp::SyncMode::Invalid;
        }

        QString syncModeToString(Ptp::SyncMode value)
        {
            switch (value) {
                case Ptp::SyncMode::OneStep:   return "OneStep";
                case Ptp::SyncMode::TwoStep:   return "TwoStep";
                default:
                    return "";
            }
        }

        Ptp::DelayMechanism delayMechanismFromString(const QStringRef &value)
        {
            if(value.contains("E2E", Qt::CaseInsensitive)) return Ptp::DelayMechanism::E2E;
            if(value.contains("P2P", Qt::CaseInsensitive)) return Ptp::DelayMechanism::P2P;
            return Ptp::DelayMechanism::Invalid;
        }

        QString delayMechanismToString(Ptp::DelayMechanism value)
        {
            switch (value) {
                case Ptp::DelayMechanism::E2E:   return "E2E";
                case Ptp::DelayMechanism::P2P:   return "P2P";
                default:
                    return "";
            }
        }

        Ptp::Protocol protocolFromString(const QStringRef &value)
        {
            if(value.contains("Ipv4", Qt::CaseInsensitive))     return Ptp::Protocol::Ipv4;
            if(value.contains("Ipv6", Qt::CaseInsensitive))     return Ptp::Protocol::Ipv6;
            if(value.contains("L2MCast1", Qt::CaseInsensitive)) return Ptp::Protocol::L2MCast1;
            if(value.contains("L2MCast2", Qt::CaseInsensitive)) return Ptp::Protocol::L2MCast2;
            return Ptp::Protocol::Invalid;
        }

        QString protocolToString(Ptp::Protocol value)
        {
            switch (value) {
                case Ptp::Protocol::Ipv4:       return "Ipv4";
                case Ptp::Protocol::Ipv6:       return "Ipv6";
                case Ptp::Protocol::L2MCast1:   return "L2MCast1";
                case Ptp::Protocol::L2MCast2:   return "L2MCast2";
                default:
                    return "";
            }
        }

        EthernetPort::PpsSource ppsSourceFromString(const QStringRef &value)
        {
            if(value.contains("MasterInternal", Qt::CaseInsensitive))     return EthernetPort::PpsSource::MasterInternal;
            if(value.contains("MasterGnss", Qt::CaseInsensitive))     return EthernetPort::PpsSource::MasterGnss;
            if(value.contains("MasterSlave", Qt::CaseInsensitive)) return EthernetPort::PpsSource::MasterSlave;
            if(value.contains("Slave", Qt::CaseInsensitive)) return EthernetPort::PpsSource::Slave;
            return EthernetPort::PpsSource::Invalid;
        }

        QString ppsSourceToString(EthernetPort::PpsSource value)
        {
            switch (value) {
                case EthernetPort::PpsSource::MasterInternal:   return "MasterInternal";
                case EthernetPort::PpsSource::MasterGnss:       return "MasterGnss";
                case EthernetPort::PpsSource::MasterSlave:      return "MasterSlave";
                case EthernetPort::PpsSource::Slave:            return "Slave";
                default:
                    return "";
            }
        }

        EthernetPort::Speed portSpeedFromString(const QStringRef &value)
        {
            if(value.contains("Auto", Qt::CaseInsensitive))         return EthernetPort::Speed::Auto;
            if(value.contains("NoCapture", Qt::CaseInsensitive))    return EthernetPort::Speed::NoCapture;
            if(value.contains("M1000", Qt::CaseInsensitive))        return EthernetPort::Speed::M1000;
            if(value.contains("M100", Qt::CaseInsensitive))         return EthernetPort::Speed::M100;
            if(value.contains("M10", Qt::CaseInsensitive))          return EthernetPort::Speed::M10;
            return EthernetPort::Speed::Invalid;
        }

        QString portSpeedToString(EthernetPort::Speed value)
        {
            switch (value) {
                case EthernetPort::Speed::Auto:     return "Auto";
                case EthernetPort::Speed::M1000:    return "1000M";
                case EthernetPort::Speed::M100:     return "100M";
                case EthernetPort::Speed::M10:      return "10M";
                default:
                    return "";
            }
        }
    }
}


