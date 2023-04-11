#ifndef USER_H
#define USER_H

#include "ssu.h"

namespace ssu {
    struct User
    {
        User(const QString &name, SSU::UserLevel level, qint64 untilTime = 0, bool connected = false);

        QString name;
        SSU::UserLevel level;
        qint64 untilTime;           // Если пользователь заблокирован, то тут будет передано время до которого заблокирован
        bool connected;
    };
}

#endif // USER_H
