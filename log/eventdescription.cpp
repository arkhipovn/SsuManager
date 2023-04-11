#include "eventdescription.h"

namespace ssu {
    EventDescription::EventDescription(int cardType, int id) :
        id_(id),
        cardType_(cardType),
        status_(SSU::EventStatus::NotAlarm)
    {

    }

    EventDescription::EventDescription(int cardType, int id, const QString &description, SSU::EventStatus status) :
        id_(id),
        cardType_(cardType),
        description_(description),
        status_(status)
    {

    }

    bool EventDescription::operator <(const EventDescription &event) const
    {
        return cardType_ == event.cardType_ ? id_ < event.id_
                                            : cardType_ < event.cardType_;
    }

    int EventDescription::id() const
    {
        return id_;
    }

    int EventDescription::cardType() const
    {
        return cardType_;
    }

    QString EventDescription::description() const
    {
        return description_;
    }

    SSU::EventStatus EventDescription::status() const
    {
        return status_;
    }

    void EventDescription::setStatus(const SSU::EventStatus &status)
    {
        status_ = status;
    }
}
