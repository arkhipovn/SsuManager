#include "card.h"

ssu::Card::Card(int type, int address, QObject *parent) : QObject(parent),
    type_(type),
    address_(address),
    leds_(nullptr)
{

}

ssu::Card::~Card()
{
    qDeleteAll(inputChannels_);
    qDeleteAll(outputChannels_);
}

QQmlListProperty<ssu::Channel> ssu::Card::inputChannels()
{
    return QQmlListProperty <Channel>(static_cast<QObject*>(this), static_cast<void*>(&inputChannels_),
                                      &Card::numberOfChannels, &Card::atChannel);
}

QQmlListProperty<ssu::Channel> ssu::Card::outputChannels()
{
    return QQmlListProperty <Channel>(static_cast<QObject*>(this), static_cast<void*>(&outputChannels_),
                                      &Card::numberOfChannels, &Card::atChannel);
}

int ssu::Card::type() const
{
    return type_;
}

int ssu::Card::address() const
{
    return address_;
}

QString ssu::Card::serialNumber() const
{
    return serialNumber_;
}

void ssu::Card::setSerialNumber(const QString &serialNumber)
{
    if(serialNumber_ != serialNumber) {
        serialNumber_ = serialNumber;
        emit serialNumberChanged();
    }
}

QString ssu::Card::software() const
{
    return software_;
}

void ssu::Card::setSoftware(const QString &software)
{
    if(software_ != software) {
        software_ = software;
        emit softwareChanged();
    }
}

QString ssu::Card::hardware() const
{
    return hardware_;
}

void ssu::Card::setHardware(const QString &hardware)
{
    if(hardware_ != hardware) {
        hardware_ = hardware;
        emit hardwareChanged();
    }
}

QString ssu::Card::slot() const
{
    return SSU::addressToString(address_);
}

bool ssu::Card::operator ==(const Card &card) const
{
    if(this->type_ != card.type_) return false;
    if(this->address_ == card.address_) return true;
    return false;
}

bool ssu::Card::operator <(Card &card)
{
    return this->address_ < card.address_;
}

ssu::Channel *ssu::Card::inputChannelAt(int index) const
{
    return inputChannels_[index];
}

ssu::Channel *ssu::Card::outputChannelAt(int index) const
{
    return outputChannels_[index];
}

int ssu::Card::numberOfChannels(QQmlListProperty<Channel> *list)
{
    auto l = static_cast <QList <Channel*> *> (list->data);
    return l->size();
}

ssu::Channel *ssu::Card::atChannel(QQmlListProperty<Channel> *list, int index)
{
    auto l = static_cast <QList <Channel*> *> (list->data);
    return l->at(index);
}

ssu::LedList *ssu::Card::leds() const
{
    return leds_;
}

QDebug operator <<(QDebug stream, const ssu::Card &card)
{
    return stream << "Card" << " " << card.type() << " " << card.address();
}

QDebug operator <<(QDebug stream, const ssu::Card *card)
{
    return stream << (*card);
}
