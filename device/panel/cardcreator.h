#ifndef CARDCREATOR_H
#define CARDCREATOR_H
#include "card.h"

// Фабричный метод

namespace ssu {
    class CardCreator
    {
    public:
        CardCreator() = default;
        virtual ~CardCreator() = default;
        virtual Card *create(int address, QObject *parent = nullptr) const = 0;
    };
}

#endif // CARDCREATOR_H
