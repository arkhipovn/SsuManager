#include "user.h"
#include <QDateTime>

ssu::User::User(const QString &name, SSU::UserLevel level, qint64 untilTime, bool connected):
    name(name),
    level(level),
    untilTime(untilTime),
    connected(connected)
{

}

