#include "vch003event.h"

QDebug operator <<(QDebug stream, const ssu::Vch003Event &e)
{
    return stream << "Event {" << e.occurredTime << e.clearedTime << e.slot << SSU::cardTypeToString(e.card) << e.eventId << e.channel << e.parameters << "}";
}

uint ssu::qHash(const ssu::Vch003Event &e, uint seed) noexcept
{
    if(e.hash != 0)
        return e.hash;
    return ::qHash(QString::number(e.occurredTime) +
                   QString(e.slot) +
                   QString::number(e.card) +
                   QString::number(e.eventId) +
                   QString::number(e.channel) +
                   QString(e.parameters).remove(QRegExp("\\s"))
                   , seed);
}

bool ssu::Vch003Event::operator ==(const ssu::Vch003Event &event) const
{
    if(hash != 0 && event.hash != 0) {
        return hash == event.hash;
    }

    if(occurredTime != event.occurredTime) return false;
    if(slot != event.slot) return false;
    if(card != event.card) return false;
    if(eventId != event.eventId) return false;
    if(channel != event.channel) return false;
    if(parameters != event.parameters) return false;
    return true;
}

bool ssu::Vch003Event::operator ==(uint hash) const
{
    if(this->hash != 0) {
        return this->hash == hash;
    }
    return qHash(*this) == hash;
}
