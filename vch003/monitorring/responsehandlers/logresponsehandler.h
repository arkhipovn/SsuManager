#ifndef LOGRESPONSEHANDLER_H
#define LOGRESPONSEHANDLER_H

#include "deviceresponsehandler.h"

namespace ssu {
    //------- Обработка сообщения получения списка событий -------
    class GetEventDescriptionReponseHandler : public DeviceResponseHandler
    {
    public:
        GetEventDescriptionReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };


    //------- Обработка сообщения установки статуса события -------
    //    class SetCardEventStatusReponseHandler : public DeviceResponseHandler
    //    {
    //    public:
    //        SetCardEventStatusReponseHandler(Device *device);
    //        bool handle(const QString &data) override;
    //    };

    //------- Обработка сообщения со списком активных событий -------
    class GetActiveEventsReponseHandler : public DeviceResponseHandler
    {
    public:
        GetActiveEventsReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с журналом событий -------
    class GetEventLogReponseHandler : public DeviceResponseHandler
    {
    public:
        GetEventLogReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };


    //------- Обработка сообщения с действиями пользователя -------

    class GetUserLogReponseHandler : public DeviceResponseHandler
    {
    public:
        GetUserLogReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения со списком сигналов -------

    class GetReclistReponseHandler : public DeviceResponseHandler
    {
    public:
        GetReclistReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };


    //------- Обработка сообщения с журналом измерений -------

    class GetMeasLogReponseHandler : public DeviceResponseHandler
    {
    public:
        GetMeasLogReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };


    //------- Обработка сообщения с текущими измерениями -------

    class GetCurrentMeasReponseHandler : public DeviceResponseHandler
    {
    public:
        GetCurrentMeasReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };



}

#endif // LOGRESPONSEHANDLER_H




