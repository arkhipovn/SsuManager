#ifndef AUTHORIZATIONRESPONSEHANDLER_H
#define AUTHORIZATIONRESPONSEHANDLER_H

#include "responsehandler.h"
#include "device/configuration/connectionsettings.h"

namespace ssu {
    class Vch003Monitoring;

    class AuthorizationResponseHandler : public ResponseHandler
    {
    public:
        AuthorizationResponseHandler(Vch003Monitoring *monitoring);
        virtual ~AuthorizationResponseHandler() = default;

    protected:
        Vch003Monitoring *monitoring_;

        ssu::ConnectionSettings::Status connectionStatus();
        void setConnectionStatus(ssu::ConnectionSettings::Status status);
        void setConnectionType(int connectionType);
    };


    //________________________

    //------- Если пришло сообщение о блокировки пользователя -------

    class UserBlockedHandler : public AuthorizationResponseHandler
    {
    public:
        UserBlockedHandler(Vch003Monitoring *monitoring);
        bool handle(const QString &data) override;
    };

    //------- Неизвестный логин -------
    class UnknownLoginHandler : public AuthorizationResponseHandler
    {
    public:
        UnknownLoginHandler(Vch003Monitoring *monitoring);
        bool handle(const QString &data) override;
    };

    //------- Неизвестный пароль -------
    class UnknownPasswordHandler : public AuthorizationResponseHandler
    {
    public:
        UnknownPasswordHandler(Vch003Monitoring *monitoring);
        bool handle(const QString &data) override;
    };

    class LoginHandler : public AuthorizationResponseHandler
    {
    public:
        LoginHandler(Vch003Monitoring *monitoring);
        bool handle(const QString &data) override;
    };

    class PasswordHandler : public AuthorizationResponseHandler
    {
    public:
        PasswordHandler(Vch003Monitoring *monitoring);

        bool handle(const QString &data) override;
    };

    class GrettingsMessageHandler : public AuthorizationResponseHandler
    {
    public:
        GrettingsMessageHandler(Vch003Monitoring *monitoring = nullptr);
        bool handle(const QString &data) override;
    };
}


#endif // AUTHORIZATIONRESPONSEHANDLER_H
