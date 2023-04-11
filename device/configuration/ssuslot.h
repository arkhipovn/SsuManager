#ifndef SSUSLOT_H
#define SSUSLOT_H

#include <QObject>
#include "slotconfiguration.h"

namespace ssu {
    class SsuSlot : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
        Q_PROPERTY(int address READ address CONSTANT)
        Q_PROPERTY(int type READ type NOTIFY typeChanged)
        Q_PROPERTY(SlotConfiguration configuration READ configuration CONSTANT)

    public:
        explicit SsuSlot(QObject *parent = nullptr);
        explicit SsuSlot(int address, const SlotConfiguration &conf, QObject *parent = nullptr);

        int type() const;
        void setType(int type);

        SlotConfiguration configuration() const;
        int address() const;

        bool isAvailable() const;
        void setAvailable(bool available);

    signals:
        void typeChanged();
        void availableChanged();

    private:
        bool available_;
        int address_;
        int type_;
        SlotConfiguration configuration_;
    };
}

#endif // SSUSLOT_H
