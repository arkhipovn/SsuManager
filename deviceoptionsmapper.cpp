#include "deviceoptionsmapper.h"

namespace ssu {
    DeviceOptionsMapper *DeviceOptionsMapper::getInstanse() const
    {
        static DeviceOptionsMapper instanse;
        return &instanse;
    }

    DeviceOptionsMapper::DeviceOptionsMapper(QObject *parent) : QObject(parent)
    {

    }

    DeviceOptions *DeviceOptionsMapper::options(int type) const
    {
        const auto it = deviceOptionsMapper.constFind(type);
        return it == deviceOptionsMapper.constEnd() ? nullptr
                                                    : (*it);
    }
}
