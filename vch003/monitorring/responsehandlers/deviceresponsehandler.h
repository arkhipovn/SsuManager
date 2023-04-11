#ifndef DEVICERESPONSEHANDLER_H
#define DEVICERESPONSEHANDLER_H

#include "responsehandler.h"
#include "device/device.h"

namespace ssu {
    class DeviceResponseHandler : public ResponseHandler {
    public:
        DeviceResponseHandler(Device *device);
        virtual ~DeviceResponseHandler() = default;

    protected:
        /*
    cmd - команда, которую нужно искать
    data - данные, которые пришли
    slot - адрес, который пришел
    command - команда, которая пришла
    request - адрес, который пришел
    */
        bool parseData(const QString &cmd, const QString &data, QString &slot, QString &command, QString &request) const;
        Device *device_;
    };
}


#endif // DEVICERESPONSEHANDLER_H
