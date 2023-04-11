#include "authorizationresponsehandler.h"
#include "vch003/monitorring/vch003monitoring.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <QDebug>

using namespace ssu;

AuthorizationResponseHandler::AuthorizationResponseHandler(Vch003Monitoring *monitoring) : ResponseHandler(),
    monitoring_(monitoring)
{

}

ssu::ConnectionSettings::Status AuthorizationResponseHandler::connectionStatus()
{
    return monitoring_->connectionSettings()->status();
}

void AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::Status status)
{
    monitoring_->connectionSettings()->setStatus(status);
}

void AuthorizationResponseHandler::setConnectionType(int connectionType)
{
    monitoring_->connectionType_ = static_cast<Vch003Monitoring::ConnectionType>(connectionType);
}


//___________ UserBlockedHandler ________

UserBlockedHandler::UserBlockedHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool UserBlockedHandler::handle(const QString &data)
{
    if(data.contains("Blocked till", Qt::CaseInsensitive)) {
        if(monitoring_->connectionType() == Vch003Monitoring::ConnectionType::OneTimeConnection) {
            // если первое подключение и не правильно ввели, выдаем
            AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::WaitingForAuthorization);
        }
        else monitoring_->disconnectFromDevice();
    }
    else
        return nextHandle(data);
    return true;
}

//_________________________________________


//___________ UnknownLoginHandler ________

UnknownLoginHandler::UnknownLoginHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool UnknownLoginHandler::handle(const QString &data)
{
    if(data.contains("Unknown login", Qt::CaseInsensitive)) {
        if(monitoring_->connectionType() == Vch003Monitoring::ConnectionType::OneTimeConnection) {
            // если первое подключение и не правильно ввели, выдаем
            AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::WaitingForAuthorization);
            monitoring_->connectionSettings()->incrementNumberOfAuthorizationAttempts();  // увеличиваем счетчик попыток авторизации
        }
        else monitoring_->disconnectFromDevice();
        monitoring_->connectionSettings()->invalidUsername();
    }
    else
        return nextHandle(data);
    return true;
}

//_________________________________________



//___________ UnknownPasswordHandler _________
UnknownPasswordHandler::UnknownPasswordHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool UnknownPasswordHandler::handle(const QString &data)
{
    if(data.contains("Unknown Password", Qt::CaseInsensitive)) {
        if(monitoring_->connectionType() == Vch003Monitoring::ConnectionType::OneTimeConnection) {
            // если первое подключение и не правильно ввели, выдаем
            AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::WaitingForPassword);
            monitoring_->connectionSettings()->incrementNumberOfAuthorizationAttempts();  // увеличиваем счетчик попыток авторизации
        }
        else monitoring_->disconnectFromDevice();
        monitoring_->connectionSettings()->invalidPassword();
    }
    else
        return nextHandle(data);
    return true;
}
//_________________________________________




//___________ LoginHandler _________
LoginHandler::LoginHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool LoginHandler::handle(const QString &data)
{
    if(data.contains("Login:", Qt::CaseInsensitive)) {
        AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::WaitingForAuthorization);
        // в случае если нужно восстановить соединение и если есть что отправлять, отправляем..
        if(!monitoring_->connectionSettings()->username().isEmpty() &&
                monitoring_->connectionType() != Vch003Monitoring::ConnectionType::OneTimeConnection) {
            monitoring_->send(new vch003request::SetLogin(monitoring_->connectionSettings()->username())); // отправляем команду
        }
    }
    else
        return nextHandle(data);
    return true;
}

//_________________________________________



//___________ PasswordHandler _________
PasswordHandler::PasswordHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool PasswordHandler::handle(const QString &data)
{
    if(data.contains("Password:", Qt::CaseInsensitive)) {
        if(AuthorizationResponseHandler::connectionStatus() != ssu::ConnectionSettings::WaitingForPassword) // если первый раз отправляем..
            monitoring_->send(new vch003request::SetPassword(monitoring_->connectionSettings()->password())); // отправляем команду
    }
    else
        return nextHandle(data);
    return true;
}
//_________________________________________



//___________ GrettingsMessageHandler _________
GrettingsMessageHandler::GrettingsMessageHandler(Vch003Monitoring *monitoring) : AuthorizationResponseHandler(monitoring) {}

bool GrettingsMessageHandler::handle(const QString &data)
{
    QRegularExpression re(GREETING_MESSAGE);
    auto match = re.match(data);
    if(match.hasMatch()) {
        if(monitoring_ != nullptr) {
            // Ура. Похоже подключились
            AuthorizationResponseHandler::setConnectionType(Vch003Monitoring::ConnectionType::Reconnecting);    // теперь нужно восстанавливать соединение..
            AuthorizationResponseHandler::setConnectionStatus(ssu::ConnectionSettings::Status::Connected);
        }
    }
    else
        return nextHandle(data);
    return true;
}

//_________________________________________






