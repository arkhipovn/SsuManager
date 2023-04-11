#ifndef MEASDATA_H
#define MEASDATA_H

#include "datacontainer.h"

namespace ssu {
    struct MeasData
    {
        int channel;
        qint64 time;
        DataContainer data;
    };
}

Q_DECLARE_METATYPE(ssu::MeasData)

#endif // MEASDATA_H

