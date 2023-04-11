#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QObject>
#include <QMutex>
#include "ssu.h"

namespace ssu {
    class ConnectionSettings : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(Status status READ status NOTIFY statusChanged)
        Q_PROPERTY(SSU::UserLevel level READ level NOTIFY levelChanged)

        Q_PROPERTY(QString username READ username NOTIFY authorizationParametersChanged)
        Q_PROPERTY(QString password READ password NOTIFY authorizationParametersChanged)

        Q_PROPERTY(int numberOfAuthorizationAttempts READ numberOfAuthorizationAttempts NOTIFY numberOfAuthorizationAttemptsChanged)

        Q_PROPERTY(bool storeUserInformation READ storeUserInformation NOTIFY storeUserInformationChanged)
        Q_PROPERTY(bool autoConnect READ autoConnect NOTIFY autoConnectChanged)

    public:
        explicit ConnectionSettings(QObject *parent = nullptr);
        virtual ~ConnectionSettings() = default;

        enum Status {
            Unconnected = 0,
            Connection,
            ConnectEstablished,         // 2
            WaitingForAuthorization,    // 3
            WaitingForPassword,         // 4. ожидание ввода пароля.. Если запустили авторизацию, но не правильно ввели пароль
            Authorization,              // 5. запущен процесс авторизации
            Disconnection,              // 6
            Connected,                  // 7
            Unknown
        };
        Q_ENUMS(Status)

        void setAuthorizationParameters(const QString &username, const QString &password);
        void clearAuthorizationSettings();

        QString username() const;
        QString password() const;
        Q_INVOKABLE void clearPassword();

        void setDefaultUserLevel();

        SSU::UserLevel level() const;
        void setLevel(SSU::UserLevel level);

        int numberOfAuthorizationAttempts() const;
        void setNumberOfAuthorizationAttempts(int numberOfAuthorizationAttempts);
        void incrementNumberOfAuthorizationAttempts();

        Status status() const;
        void setStatus(const Status &status);

        bool autoConnect() const;
        void setAutoConnect(bool autoConnect);

        bool storeUserInformation() const;
        void setStoreUserInformation(bool storeUserInformation);

        Q_INVOKABLE virtual QString name() const;

    signals:
        void storeUserInformationChanged();
        void autoConnectChanged();
        void authorizationParametersChanged();
        void levelChanged();
        void statusChanged();

        void deviceConnected();
        void deviceDisconnected();

        void deviceNotResponding();

        void connectionEstablished();
        void connectionNotEstablished();

        void waitingForAuthorization(); // ожидаем авторизации
        void invalidUsername();         // ошибка авторизации
        void invalidPassword();         // ошибка авторизации
        void userBlocked();             // пользователь заблокирован

        void numberOfAuthorizationAttemptsChanged();

    protected:
        Status status_;

        QString username_;
        QString password_;
        SSU::UserLevel level_;

        int numberOfAuthorizationAttempts_;     // кол-во неудачных попыток подключения

        bool autoConnect_;                           // восстанавливать соединение при разрыве
        bool storeUserInformation_;                  // запоминать инфомарцию о пользователе
    };
}

#endif // CONNECTIONSETTINGS_H
