#ifndef SLOTCONFIGURATION_H
#define SLOTCONFIGURATION_H

#include <QObject>
#include "ssu.h"

namespace ssu {
    class SlotConfiguration
    {
        Q_GADGET

        Q_PROPERTY(QList <int> types READ types CONSTANT)
        Q_PROPERTY(int groupNumber READ groupNumber CONSTANT)

    public:
        SlotConfiguration();
        SlotConfiguration(SSU::CardType type, int groupNumber = 0);
        SlotConfiguration(const QList <SSU::CardType> &types, int groupNumber = 0);

        QList <int> types() const;
        int groupNumber() const;

        bool isEmpty();
        int type(int i) const;

        Q_INVOKABLE bool contains(int type) const;

    private:
        int groupNumber_;               // Группа. У некоторых устройств платы идут парами. В этом случае и нужно передавать одну и туже группу
        QList <int> types_;             // SSU::CardType // Какие платы могут быть в данном слоте
    };
}

Q_DECLARE_METATYPE(ssu::SlotConfiguration)


#endif // SLOTCONFIGURATION_H
