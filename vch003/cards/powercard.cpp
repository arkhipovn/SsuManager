#include "powercard.h"

namespace ssu {
    PowerCard::PowerCard(int address, QObject *parent) : Vch003Card(SSU::Power, address, parent),
        supply_(true),
        fuse_(true),
        cardActive_(true)
    {
        leds_ = new LedList({
                                Qt::darkGreen,     // Питание
                                Qt::darkGreen      // Предохр
                            },
                            this);
    }

    bool PowerCard::supply() const
    {
        return supply_;
    }

    void PowerCard::setSupply(bool supply)
    {
        if(supply_ != supply) {
            supply_ = supply;
            emit supplyChanged();
        }
        leds_->at(0)->setState(true, supply_ ? Qt::darkGreen : Qt::red);
        leds_->at(1)->setState(supply, fuse_ ? Qt::darkGreen : Qt::red);
    }

    bool PowerCard::fuse() const
    {
        return fuse_;
    }

    void PowerCard::setFuse(bool fuse)
    {
        if(fuse_ != fuse) {
            fuse_ = fuse;
            emit fuseChanged();
        }
        leds_->at(1)->setState(supply_, fuse_ ? Qt::darkGreen : Qt::red);
    }

    bool PowerCard::cardActive() const
    {
        return cardActive_;
    }

    void PowerCard::setCardActive(bool cardActive)
    {
        if(cardActive_ != cardActive) {
            cardActive_ = cardActive;
            emit cardActiveChanged();
        }
    }
}
