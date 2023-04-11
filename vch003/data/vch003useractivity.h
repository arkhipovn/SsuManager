#ifndef VCH003USERACTIVITY_H
#define VCH003USERACTIVITY_H

#include <QDebug>
#include "ssu.h"

namespace ssu {
    struct Vch003UserActivity {
        qint64 occurredTime;
        QString address;            // IP
        QString username;
        QString action;
    };

    uint qHash(const Vch003UserActivity &a, uint seed = 0) noexcept;
}

Q_DECLARE_METATYPE(ssu::Vch003UserActivity)
QDebug operator <<(QDebug stream, const ssu::Vch003UserActivity &a);

#endif // VCH003USERACTIVITY_H



