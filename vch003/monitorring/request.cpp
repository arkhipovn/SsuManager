#include "request.h"
#include "ssu.h"

namespace ssu {
    Request::Request() :
        slot_(""),
        command_(""),
        arg_(""),
        delay_(0),
        responseTime_(SSU::kResponseTime)
    {
        //    qInfo() << "Request()" << this;
    }

    Request::Request(const QString &command, const QString &arg, const QString &slot) :
        slot_(slot),
        command_(command),
        arg_(arg),
        delay_(0),
        responseTime_(SSU::kResponseTime)
    {
        //    qInfo() << "Request" << this << slot_ << command_ << arg_;
    }

    Request::SendResult Request::send(TcpReader *reader)
    {
        auto str = this->toString();
        return qMakePair(reader->write(str),
                         std::move(str));
    }

    QString Request::toString() const
    {
        QString msg = requestWithoutArguments();
        auto argument = arg();
        if(!argument.isEmpty()) msg.append(" " + argument);
        return msg;
    }

    bool Request::operator ==(const Request &other) const
    {
        if(slot_.compare(other.slot_, Qt::CaseInsensitive) != 0)
            return false;
        if(command_.compare(other.command_, Qt::CaseInsensitive) != 0)
            return false;

        if(arg_.isEmpty() && other.textArg().isEmpty()) //@N подумать насчет этого..
            return true;

        if(arg().compare(other.arg(), Qt::CaseInsensitive) != 0)
            return false;

        return true;
    }

    bool Request::operator ==(const QString &other) const
    {
        return toString().compare(other, Qt::CaseInsensitive) == 0;
    }

    Request &Request::operator=(const Request &other)
    {
        if(this == &other)
            return *this;
        slot_ = other.slot_;
        command_ = other.command_;
        arg_ = other.arg_;

        delay_ = other.delay_;
        timer_ = other.timer_;
        responseTime_ = other.responseTime_;
        return *this;
    }

    Request* Request::execute(bool success)
    {
        Q_UNUSED(success)
        return nullptr;
    }

    void Request::start()
    {
        if(timer_.isValid())
            return;
        timer_.start();
    }

    void Request::restart()
    {
        timer_.restart();
    }

    bool Request::timeout()
    {
        return timer_.hasExpired(responseTime_);
    }

    bool Request::hasSend()
    {
        return timer_.hasExpired(delay_);
    }

    qint64 Request::responseTime() const
    {
        return responseTime_;
    }

    void Request::setResponseTime(const qint64 &responseTime)
    {
        responseTime_ = responseTime;
    }

    qint64 Request::delay() const
    {
        return delay_;
    }

    void Request::setDelay(const qint64 &delay)
    {
        delay_ = std::abs(delay);
    }

    QString Request::slot() const
    {
        return slot_;
    }

    QString Request::command() const
    {
        return command_;
    }

    QString Request::arg() const
    {
        return arg_;
    }

    QString Request::requestWithoutArguments() const
    {
        QString msg = slot_;
        if(!msg.isEmpty()) msg.append(" ");
        msg += command_;
        return msg;
    }

    void Request::setTimer(const QElapsedTimer &timer)
    {
        timer_ = timer;
    }

    QElapsedTimer Request::timer() const
    {
        return timer_;
    }

    QString Request::textArg() const
    {
        return arg_;
    }

    Request::operator QString() const
    {
        return toString();
    }
}
