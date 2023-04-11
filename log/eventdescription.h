#ifndef EVENTDESCRIPTION_H
#define EVENTDESCRIPTION_H

#include "ssu.h"

namespace ssu {
    class EventDescription
    {
    public:
        EventDescription(int cardType, int id);
        EventDescription(int cardType, int id, const QString &description, SSU::EventStatus status);

        bool operator < (const EventDescription &event) const;

        int id() const;
        int cardType() const;
        QString description() const;

        SSU::EventStatus status() const;
        void setStatus(const SSU::EventStatus &status);

    private:
        int id_;
        int cardType_;
        QString description_;
        SSU::EventStatus status_;
    };
}

#endif // EVENTDESCRIPTION_H
