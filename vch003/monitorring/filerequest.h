#ifndef GETFILEREQUEST_H
#define GETFILEREQUEST_H

#include "request.h"

namespace ssu {
    class Device;
    //------- Интерфейс команды считывания файла -------

    //@N переделать бы все, что связано с устройством (Device)..

    class GetFileRequest : public Request
    {
    public:
        GetFileRequest(const QString &path, const QString &fileName,
                       const QString &command, const QString &arg = "",
                       const QString &address = "", Device *device = nullptr);
        virtual ~GetFileRequest() = default;
        SendResult send(TcpReader *reader) override;
        QString absoluteFilePath() const;
        Request *execute(bool success) override;

    protected:
        QString path_;
        QString fileName_;
        Device *device_;
    };


    //------- Интерфейс команды отправки файла -------

    class SetFileRequest : public Request
    {
    public:
        SetFileRequest(const QString &path, const QString &fileName,
                       const QString &command, const QString &arg = "",
                       const QString &address = "", Device *device = nullptr);
        virtual ~SetFileRequest() = default;

        SendResult send(TcpReader *reader) override;
        Request *execute(bool success) override;
        QString absoluteFilePath() const;

    protected:
        QString path_;
        QString fileName_;
        Device *device_;

        void delay(int ms);
    };
}

#endif // GETFILEREQUEST_H
