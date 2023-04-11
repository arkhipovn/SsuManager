#ifndef RESPONSEHANDLER_H
#define RESPONSEHANDLER_H

#include <QString>
#include <QStringRef>

// Цепочка обязанностей
namespace ssu {
    class ResponseHandler
    {
    public:
        ResponseHandler();
        virtual ~ResponseHandler();

        void setNext(ResponseHandler *next);
        virtual bool handle(const QString &data) = 0;
    protected:
        bool nextHandle(const QString &data);
        ResponseHandler *next_;
    };
}

#endif // RESPONSEHANDLER_H
