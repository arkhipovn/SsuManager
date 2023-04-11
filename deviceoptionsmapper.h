#ifndef DEVICEOPTIONSMAPPER_H
#define DEVICEOPTIONSMAPPER_H

#include <QHash>
#include <QObject>

#include "vch003/configuration/vch003options.h"

// Класс-обертка с параметрами для устройств

//@N test class

namespace ssu {
    class DeviceOptionsMapper : public QObject
    {
        Q_OBJECT

    public:
        DeviceOptionsMapper *getInstanse() const;

    private:
        explicit DeviceOptionsMapper(QObject *parent = nullptr);
        ~DeviceOptionsMapper() = default;

        DeviceOptions *options(int type) const;

        const QHash <int, DeviceOptions*> deviceOptionsMapper = {
            { static_cast<int>(SSU::DeviceType::Vch003), new Vch003DeviceOptions }
        };
    };
}

#endif // DEVICEOPTIONSMAPPER_H
