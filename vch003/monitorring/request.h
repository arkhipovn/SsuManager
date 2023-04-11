#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QElapsedTimer>
#include <functional>
#include <QObject>

#include "tcpreader.h"

#include <QDebug>

namespace ssu {
    class Request
    {
    public:
        Request();
        Request(const QString &command, const QString &arg = "", const QString &slot = "");
        virtual ~Request() = default;
//         virtual ~Request() { qInfo() << "~Request" << this << slot_ << command_ << arg_; }

        using SendResult = QPair <bool, QString>;

        virtual SendResult send(TcpReader *reader);     // Если что-то было отправлено, то вернется запрос. Если нет, то пустая строка
        virtual Request *execute(bool success);         // Если нужно отправить еще команду, то вернем указатель

        virtual QString toString() const;
        operator QString() const;

        bool operator ==(const Request &other) const;
        bool operator ==(const QString &other) const;

        Request& operator=(const Request &other);

        void start();
        void restart();

        bool timeout();     // Истекло ли время ожидания
        bool hasSend();     // Нужно ли отправить команду

        qint64 responseTime() const;
        void setResponseTime(const qint64 &responseTime);

        qint64 delay() const;
        void setDelay(const qint64 &delay);

        QString slot() const;
        QString command() const;
        virtual QString arg() const;

        QString requestWithoutArguments() const;

        void setTimer(const QElapsedTimer &timer);
        QElapsedTimer timer() const;

        QString textArg() const;    // Возврат arg_. Тк в метод arg() можно переопределить

    protected:
        QString slot_;
        QString command_;
        QString arg_;

        qint64 delay_;                  // через столько нужно отправить команду
        QElapsedTimer timer_;           // таймер для ожидания ответа
        qint64 responseTime_;           // мc, время ожидания ответа от девайса
    };


    using SharedRequest = QSharedPointer<Request>;
}

#endif // REQUEST_H

