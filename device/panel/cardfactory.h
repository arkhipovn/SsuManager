#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include <QHash>

#include "cardcreator.h"

namespace ssu {
    class CardFactory
    {
    public:
        CardFactory();
        CardFactory(QHash <int, CardCreator*> factory);
        ~CardFactory();

        void insertCreator(int type, CardCreator *creator);

        Card *create(const CardInfo &info, QObject *parent = nullptr) const;

        // Возможные типы плат в системе
        QList <int> types() const;  // SSU::CardType

    private:
        QHash <int, CardCreator*> factory_;  // SSU::CardType
    };
}

#endif // CARDFACTORY_H
