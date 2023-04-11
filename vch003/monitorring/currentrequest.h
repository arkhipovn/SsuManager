#ifndef CURRENTREQUEST_H
#define CURRENTREQUEST_H

#include <QMutex>
#include <QSharedPointer>
#include "request.h"

namespace ssu {
    class CurrentRequest {
    public:
        CurrentRequest(QMutex *mutex);

        enum class Status {
            Null,       // Отправленных сообщений нет
            Timeout,    // Есть отправленное сообщение и вышло время ожидания
            Ok,
            Unknown
        };

        Status status() const;
        bool isNull() const;
        void send(TcpReader *reader);
        Request *execute(bool success);
        void clear();

        QString toString() const;
        void setRequest(const SharedRequest &request);

        int numberOfAttemptsToSendMessages() const;
        void resetNumberOfAttemptsToSendMessages();
        int incrementNumberOfAttemptsToSendMessages();

    private:
        mutable QMutex *mutex_;
        SharedRequest request_;                 // текущее отправленное сообщение
        QString stringRequest_;                 // текущее отправленное сообщение в виде строки
        int numberOfAttemptsToSendMessages_;    // кол-во попыток отправки сообщения
    };
}

#endif // CURRENTREQUEST_H
