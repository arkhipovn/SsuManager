#include "usercontrolresponsehandler.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "vch003/monitorring/vch003requests.h"
#include "vch003/vch003.h"
#include "vch003/vch003device.h"
#include "device/user/user.h"

ssu::UserListReponseHandler::UserListReponseHandler(ssu::Device *device) : DeviceResponseHandler(device) {}

bool ssu::UserListReponseHandler::handle(const QString &data)
{
    /*
get userslist
28.12.2022 07:37:33, VCH-003, VCH003_NAME
ADMIN, Administrator, 0, Disconnected
SEC, Security, 0, Connected
USER, User, 15.09.2022 15:05:00, Disconnected
USER_NAME, Administrator, 14.09.2022 18:00:00, Disconnected

SEC@VCH003_NAME->
28.12.2022 07:37:34, VCH-003, VCH003_NAME
TRAP, A11, CardOutput, , 6, CRITICAL
SEC@VCH003_NAME->
28.12.2022 07:37:34, VCH-003, VCH003_NAME
*/

    QString slot, command, request;
    if(parseData(vch003request::GetUserList::cmd(), data, slot, command, request)) {
        QRegularExpression re("([^,\\s]+), (" +
                              vch003request::userLevelToString(SSU::UserLevel::User) + "|" +
                              vch003request::userLevelToString(SSU::UserLevel::Admin) + "|" +
                              vch003request::userLevelToString(SSU::UserLevel::Security) +
                              "), (" + REGEXP_TIME + "|0" + "), (Disconnected|Connected)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;
        QVector <User> users;

        QTextStream stream(&request, QIODevice::ReadOnly);

        QString line;
        while(stream.readLineInto(&line)) {
            if(line.isEmpty())
                continue;

            match = re.match(line);

            if(!match.hasMatch()) continue;

            User user(match.captured(1),
                      vch003request::userLevelFromString(match.capturedRef(2)));
            if(user.name.isEmpty()) continue;

            auto untilTimeString = match.captured(3);
            if(untilTimeString == "0") {
                user.untilTime = 0;
            }
            else {
                QDateTime time = QDateTime::fromString(untilTimeString, DEVICE_TIME_FORMAT);
                if(time.isValid())
                    time.setTimeSpec(Qt::UTC);
                user.untilTime = time.toSecsSinceEpoch();
            }

            user.connected = match.captured(4) == "Connected";
            users.append(user);
        }
        device_->userList()->set(users);
    }
    else return nextHandle(data);
    return true;
}

//-----------------------------------------------------------------------------------------------------

ssu::AddUserReponseHandler::AddUserReponseHandler(ssu::Device *device) : DeviceResponseHandler(device) {}

bool ssu::AddUserReponseHandler::handle(const QString &data)
{
    QString slot, command, request;
    if(parseData(vch003request::AddUser::cmd(), data, slot, command, request)) {
        qobject_cast<ssu::Vch003Device*>(device_)->monitoring()->send(new vch003request::GetUserList);  //@N по событию бы потом реагировать..
    }
    else return nextHandle(data);
    return true;
}

//-----------------------------------------------------------------------------------------------------

ssu::DeleteUserReponseHandler::DeleteUserReponseHandler(ssu::Device *device) : DeviceResponseHandler(device) {}

bool ssu::DeleteUserReponseHandler::handle(const QString &data)
{
    QString slot, command, request;
    if(parseData(vch003request::DeleteUser::cmd(), data, slot, command, request)) {
        qobject_cast<ssu::Vch003Device*>(device_)->monitoring()->send(new vch003request::GetUserList);  //@N по событию бы потом реагировать..
    }
    else return nextHandle(data);
    return true;
}

//-----------------------------------------------------------------------------------------------------

ssu::ChangeUserPasswordReponseHandler::ChangeUserPasswordReponseHandler(ssu::Device *device) : DeviceResponseHandler(device) {}

bool ssu::ChangeUserPasswordReponseHandler::handle(const QString &data)
{
    QString slot, command, request;
    if(parseData(vch003request::SetUserPassword::cmd(), data, slot, command, request)) {
        qobject_cast<ssu::Vch003Device*>(device_)->monitoring()->send(new vch003request::GetUserList);  //@N по событию бы потом реагировать..
    }
    else return nextHandle(data);
    return true;
}

//-----------------------------------------------------------------------------------------------------

ssu::BlockUserReponseHandler::BlockUserReponseHandler(ssu::Device *device) : DeviceResponseHandler(device) {}

bool ssu::BlockUserReponseHandler::handle(const QString &data)
{
    QString slot, command, request;
    if(parseData(vch003request::BlockUser::cmd(), data, slot, command, request)) {
        qobject_cast<ssu::Vch003Device*>(device_)->monitoring()->send(new vch003request::GetUserList);  //@N по событию бы потом реагировать..
    }
    else return nextHandle(data);
    return true;
}

