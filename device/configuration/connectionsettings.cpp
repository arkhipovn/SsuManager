#include "connectionsettings.h"
#include <QDebug>

ssu::ConnectionSettings::ConnectionSettings(QObject *parent) : QObject(parent),
    status_(Status::Unconnected),
    level_(SSU::UserLevel::User),
    numberOfAuthorizationAttempts_(0),
    autoConnect_(true),
    storeUserInformation_(false)
{

}

void ssu::ConnectionSettings::setAuthorizationParameters(const QString &username, const QString &password)
{
    bool changed = false;
    if(username_ != username) {
        level_ = SSU::UserLevel::User;
        username_ = username;
        changed = true;
    }
    if(password_ != password) {
        password_ = password;
        changed = true;
    }
    if(changed)
        emit authorizationParametersChanged();
}

void ssu::ConnectionSettings::clearAuthorizationSettings()
{
    setDefaultUserLevel();
    setAuthorizationParameters("", "");
}

QString ssu::ConnectionSettings::username() const
{
    return username_;
}

QString ssu::ConnectionSettings::password() const
{
    return password_;
}

void ssu::ConnectionSettings::clearPassword()
{
    password_.clear();
    emit authorizationParametersChanged();
}

void ssu::ConnectionSettings::setDefaultUserLevel()
{
    setLevel(SSU::UserLevel::User);
}

SSU::UserLevel ssu::ConnectionSettings::level() const
{
    return level_;
}

void ssu::ConnectionSettings::setLevel(SSU::UserLevel level)
{
    if(level_ != level) {
        level_ = level;
        emit levelChanged();
    }
}

int ssu::ConnectionSettings::numberOfAuthorizationAttempts() const
{
    return numberOfAuthorizationAttempts_;
}

void ssu::ConnectionSettings::setNumberOfAuthorizationAttempts(int numberOfAuthorizationAttempts)
{
    if(numberOfAuthorizationAttempts_ != numberOfAuthorizationAttempts) {
        numberOfAuthorizationAttempts_ = numberOfAuthorizationAttempts;
        emit numberOfAuthorizationAttemptsChanged();
    }
}

void ssu::ConnectionSettings::incrementNumberOfAuthorizationAttempts()
{
    setNumberOfAuthorizationAttempts(numberOfAuthorizationAttempts_ + 1);
}

ssu::ConnectionSettings::Status ssu::ConnectionSettings::status() const
{
    return status_;
}

void ssu::ConnectionSettings::setStatus(const Status &status)
{
    if(status_ != status) {
        switch (status) {
            case Status::Unconnected: {
                setNumberOfAuthorizationAttempts(0);    // сбрасываем
                switch (status_) {
                    case Status::Connection:
                        emit connectionNotEstablished(); break;
                    case Status::ConnectEstablished:
                    case Status::WaitingForAuthorization:
                    case Status::Authorization:
                    case Status::Disconnection:
                    case Status::Connected:
                        emit deviceDisconnected(); break;
                    default:
                        break;
                }
                break;
            }
            case Status::ConnectEstablished:
                if(status_ == Status::Connection) {
                    setNumberOfAuthorizationAttempts(0);    // сбрасываем
                    emit connectionEstablished();
                }
                break;
            case Status::WaitingForAuthorization:
                if(status_ == Status::ConnectEstablished && username_.isEmpty())
                    emit waitingForAuthorization();
                break;
            case Status::Connected:
                emit deviceConnected(); break;
            default: break;
        }

        status_ = status;
        emit statusChanged();
    }
}

bool ssu::ConnectionSettings::autoConnect() const
{
    return autoConnect_;
}

void ssu::ConnectionSettings::setAutoConnect(bool autoConnect)
{
    if(autoConnect_ != autoConnect) {
        autoConnect_ = autoConnect;
        emit autoConnectChanged();
    }
}

bool ssu::ConnectionSettings::storeUserInformation() const
{
    return storeUserInformation_;
}

void ssu::ConnectionSettings::setStoreUserInformation(bool storeUserInformation)
{
    if(storeUserInformation_ != storeUserInformation) {
        storeUserInformation_ = storeUserInformation;
        emit storeUserInformationChanged();
    }
}

QString ssu::ConnectionSettings::name() const
{
    return "";
}
