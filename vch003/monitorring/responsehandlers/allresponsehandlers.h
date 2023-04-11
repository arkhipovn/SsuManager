#ifndef RESPONSEHANDLERS_H
#define RESPONSEHANDLERS_H

#include "authorizationresponsehandler.h"
#include "usercontrolresponsehandler.h"
#include "logresponsehandler.h"

namespace ssu {
    //------- Обработка автоматического сообщения  -------
    class TrapParseHandler : public DeviceResponseHandler
    {
    public:
        TrapParseHandler(Device *device);
        bool handle(const QString &data) override;
    private:
        void analyzeTrap(const QString &slot, SSU::CardType type, int eventId, bool atEnd);
    };

    //------- Обработка получения информации о пользователе -------
    class GetCurrentUserInfoResponseHandler : public DeviceResponseHandler
    {
    public:
        GetCurrentUserInfoResponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения инветаризации плат -------
    class InventoryResponseHandler : public DeviceResponseHandler
    {
    public:
        InventoryResponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с параметрами плат -------
    class GetParametersReponseHandler : public DeviceResponseHandler {
    public:
        GetParametersReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения со списком спутников -------
    class GetSatelliteListReponseHandler : public DeviceResponseHandler {
    public:
        GetSatelliteListReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с настроеками порта в ГНСС плате -------
    class GetGnssPortSettingsReponseHandler : public DeviceResponseHandler {
    public:
        GetGnssPortSettingsReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с статистикой порта в ГНСС плате -------
    class GetGnssPortStatisticsReponseHandler : public DeviceResponseHandler {
    public:
        GetGnssPortStatisticsReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с конфигурациями в ГНСС плате -------
    class GetGnssNetworkConfigurationReponseHandler : public DeviceResponseHandler {
    public:
        GetGnssNetworkConfigurationReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };



    //------- Конфигурация панели -------
    class GetPanelConfigReponseHandler : public DeviceResponseHandler {
    public:
        GetPanelConfigReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Сброс измерений -------
    class MeasResetReponseHandler : public DeviceResponseHandler {
    public:
        MeasResetReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //------- Обработка сообщения с пороговыми значениями -------
    class GetThresholdReponseHandler : public DeviceResponseHandler {
    public:
        GetThresholdReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    class SetAllThresholdsReponseHandler : public DeviceResponseHandler {
    public:
        SetAllThresholdsReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    class ResetThresholdsReponseHandler : public DeviceResponseHandler {
    public:
        ResetThresholdsReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };




    //------- Обработка сообщения -------
    //------- Обработка сообщения -------

}



#endif // RESPONSEHANDLERS_H
