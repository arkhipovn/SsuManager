#include "vch003options.h"
#include "vch003userrights.h"
#include <math.h>

namespace ssu {
    //@N Пока что сделаю так.. Не знаю как правильно лучше..
    const QList <Vch003DeviceOptions::InputLinkedChannels> Vch003DeviceOptions::inputLinkedChannels = {
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 },
        { 8, 12 },
        { 9, 13 },
        { 10, 14 },
        { 11, 15 },
        { 16, 17 }
    };

    Vch003DeviceOptions::Vch003DeviceOptions(QObject *parent) :
        DeviceOptions(21,
    { 1, 10, 100, 1000, 10000 },
    { 1, 10, 100, 1000 },
                      4, parent)
    {
        userRights_ = new Vch003UserRights(this);
    }
}
