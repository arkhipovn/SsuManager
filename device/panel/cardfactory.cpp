#include "cardfactory.h"

ssu::CardFactory::CardFactory()
{

}

ssu::CardFactory::CardFactory(QHash<int, CardCreator *> factory) : factory_(factory)
{

}

ssu::CardFactory::~CardFactory()
{
    auto it = factory_.begin();
    while(it != factory_.end()) {
        delete it.value();
        ++it;
    }
}

void ssu::CardFactory::insertCreator(int type, CardCreator *creator)
{
    factory_.insert(type, creator);
}

ssu::Card *ssu::CardFactory::create(const CardInfo &info, QObject *parent) const
{
    if(info.type == SSU::Stub)
        return new Card(SSU::Stub, info.address, parent);

    auto it = factory_.constFind(info.type);

    Card *card = (it == factory_.constEnd()) ? new Card(SSU::InvalidType, info.address, parent)   // А нужно ли это?
                                             : it.value()->create(info.address, parent);

    if(card != nullptr) {
        card->setSerialNumber(info.serialNumber);
        card->setSoftware(info.software);
        card->setHardware(info.hardware);
    }

    return card;
}

QList<int> ssu::CardFactory::types() const
{
    return factory_.keys();
}
