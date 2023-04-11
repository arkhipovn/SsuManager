#ifndef USERCONTROLRESPONSEHANDLER_H
#define USERCONTROLRESPONSEHANDLER_H

#include "deviceresponsehandler.h"

//GetUserList

namespace ssu {
    //------- Управление пользователями -------

    class UserListReponseHandler : public DeviceResponseHandler {
    public:
        UserListReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //-------  -------

    class AddUserReponseHandler : public DeviceResponseHandler {
    public:
        AddUserReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //-------  -------
    class DeleteUserReponseHandler : public DeviceResponseHandler {
    public:
        DeleteUserReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //-------  -------
    class ChangeUserPasswordReponseHandler : public DeviceResponseHandler {
    public:
        ChangeUserPasswordReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };

    //-------  -------
    class BlockUserReponseHandler : public DeviceResponseHandler {
    public:
        BlockUserReponseHandler(Device *device);
        bool handle(const QString &data) override;
    };
}


#endif // USERCONTROLRESPONSEHANDLER_H
