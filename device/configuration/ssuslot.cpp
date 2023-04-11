#include "ssuslot.h"

ssu::SsuSlot::SsuSlot(QObject *parent) : QObject(parent),
    available_(true),
    address_(0),
    type_(SSU::CardType::InvalidType)
{

}

ssu::SsuSlot::SsuSlot(int address, const ssu::SlotConfiguration &conf, QObject *parent) : QObject(parent),
    available_(true),
    address_(address),
    configuration_(conf)
{
    type_ = configuration_.isEmpty() ? SSU::CardType::InvalidType
                                     : configuration_.type(0);
}

ssu::SlotConfiguration ssu::SsuSlot::configuration() const
{
    return configuration_;
}

int ssu::SsuSlot::type() const
{
    return type_;
}

void ssu::SsuSlot::setType(int type)
{
    if(type_ != type) {
        type_ = type;
        emit typeChanged();
    }
}

int ssu::SsuSlot::address() const
{
    return address_;
}

bool ssu::SsuSlot::isAvailable() const
{
    return available_;
}

void ssu::SsuSlot::setAvailable(bool available)
{
    if(available_ != available) {
        available_ = available;
        emit availableChanged();
    }
}
