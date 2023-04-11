#ifndef POWERCARD_H
#define POWERCARD_H

#include "vch003card.h"

namespace ssu {
    class PowerCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(bool fuse READ fuse NOTIFY fuseChanged)
        Q_PROPERTY(bool cardActive READ cardActive NOTIFY cardActiveChanged)
        Q_PROPERTY(bool supply READ supply NOTIFY supplyChanged)

    public:
        explicit PowerCard(int address = -1, QObject *parent = nullptr);

        bool supply() const;
        void setSupply(bool supply);

        bool fuse() const;
        void setFuse(bool fuse);

        bool cardActive() const;
        void setCardActive(bool cardActive);

    signals:
        void supplyChanged();
        void fuseChanged();
        void cardActiveChanged();

    private:
        bool supply_;       // есть питание или нет
        bool fuse_;         // предохранитель
        bool cardActive_;
    };
}

#endif // POWERCARD_H
