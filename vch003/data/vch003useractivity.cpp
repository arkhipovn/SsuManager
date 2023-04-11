#include "vch003useractivity.h"

uint ssu::qHash(const ssu::Vch003UserActivity &a, uint seed) noexcept
{
    return ::qHash(QString::number(a.occurredTime) +
                   a.address +
                   a.username +
                   QString(a.action).remove(QRegExp("\\s"))
                   , seed);
}

QDebug operator <<(QDebug stream, const ssu::Vch003UserActivity &a)
{
    return stream << "Event {" << a.occurredTime << a.address << a.action << a.username << "}";
}

