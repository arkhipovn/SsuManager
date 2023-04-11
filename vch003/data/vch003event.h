#ifndef VCH003EVENT_H
#define VCH003EVENT_H

#include <QDebug>
#include "ssu.h"

namespace ssu {
    struct Vch003Event
    {
        uint hash = 0;

        qint64 occurredTime = 0;
        qint64 clearedTime = 0;

        QString slot;
        SSU::CardType card = SSU::CardType::InvalidType;

        int eventId = -1;
        int channel = -1;

        QString parameters;
        QString comment;

        bool readed = true;     // Прочитано или нет..

        bool operator == (const Vch003Event &event) const;
        bool operator == (uint hash) const;
    };

    uint qHash(const Vch003Event &e, uint seed = 0) noexcept;
}

Q_DECLARE_METATYPE(ssu::Vch003Event)
QDebug operator <<(QDebug stream, const ssu::Vch003Event &e);


#endif // VCH003EVENT_H



