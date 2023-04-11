#include "currentrequest.h"
#include <QDebug>

#include "ssu.h"

namespace ssu {
    CurrentRequest::CurrentRequest(QMutex *mutex) :
        mutex_(mutex),
        request_(nullptr),
        numberOfAttemptsToSendMessages_(0)
    {

    }

    CurrentRequest::Status CurrentRequest::status() const
    {
        CurrentRequest::Status status = Status::Unknown;
        if(mutex_->tryLock(SSU::kMutexTimeout)) {
            if(request_.isNull()) {
                status = Status::Null;
            }
            else {
                status = request_->timeout() ? Status::Timeout
                                             : Status::Ok;
            }
            mutex_->unlock();
        }
        return status;
    }

    bool CurrentRequest::isNull() const
    {
        bool isNull = false;
        if(mutex_->tryLock(SSU::kMutexTimeout)) {
            isNull = request_.isNull();
            mutex_->unlock();
        }
        return isNull;
    }

    void CurrentRequest::send(TcpReader *reader)
    {
        if(mutex_->tryLock(SSU::kMutexTimeout)) {
            if(!request_.isNull()) {
                Request::SendResult result = request_->send(reader);
                if(result.first) {
                    stringRequest_ = result.second;     // Запоминаем
                    request_->restart();                // Если отправили, запускаем ожидание
                    qDebug() << "CurrentRequest sended";
                }
                else if(numberOfAttemptsToSendMessages_ == 0) {
                    // Что-то явно не так.. Удалим сообщение..
                    execute(false);
                }
            }

            mutex_->unlock();
        }
    }

    Request *CurrentRequest::execute(bool success)
    {
        Request *newRequest = nullptr;

        if(mutex_->tryLock(SSU::kMutexTimeout)) {
            qDebug() << "CurrentRequest::execute start" << stringRequest_;

            if(!request_.isNull()) {
                newRequest = request_->execute(success);
                request_->restart();
            }

            numberOfAttemptsToSendMessages_ = 0;        // Сбрасываем
            request_.clear();
            stringRequest_.clear();

            qDebug() << "CurrentRequest::execute end" << newRequest;
            mutex_->unlock();
        }
        return newRequest;
    }

    void CurrentRequest::clear()
    {
        if(mutex_->tryLock(SSU::kMutexTimeout)) {
            numberOfAttemptsToSendMessages_ = 0;        // Сбрасываем
            request_.clear();
            stringRequest_.clear();
            mutex_->unlock();
        }
    }

    QString CurrentRequest::toString() const
    {
        return stringRequest_;
    }

    void CurrentRequest::setRequest(const SharedRequest &request)
    {
        //@N а не нужно вызвать метод execute? Вроде нет, но мб лишнем не будет?
        numberOfAttemptsToSendMessages_ = 0;
        request_ = request;
    }

    int CurrentRequest::numberOfAttemptsToSendMessages() const
    {
        return numberOfAttemptsToSendMessages_;
    }

    void CurrentRequest::resetNumberOfAttemptsToSendMessages()
    {
        numberOfAttemptsToSendMessages_ = 0;
    }

    int CurrentRequest::incrementNumberOfAttemptsToSendMessages()
    {
        return ++numberOfAttemptsToSendMessages_;
    }



}
