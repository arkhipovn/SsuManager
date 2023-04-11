#include "responsehandler.h"

#include <QDebug>

namespace ssu {

    ResponseHandler::ResponseHandler() : next_(nullptr)
    {

    }

    ResponseHandler::~ResponseHandler()
    {
        if(next_ != nullptr) delete next_;
    }

    void ResponseHandler::setNext(ResponseHandler *next)
    {
        next_ = next;
    }

    bool ResponseHandler::nextHandle(const QString &data)
    {
        if(next_ == nullptr)
            return false;
        return next_->handle(data);
    }

}
