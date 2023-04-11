#ifndef MASKDATA_H
#define MASKDATA_H

#include "maskrecommendation.h"
#include "data/datacontainer.h"

namespace ssu {
    struct MaskData
    {
    private:
        Q_GADGET
        Q_PROPERTY(QString name MEMBER name)
        Q_PROPERTY(ssu::DataContainer data MEMBER data)

    public:
        MaskData(const QString &name = "");
        MaskData(const QString &name, const DataContainer &data);
        MaskData(MaskRecommendation *recommendation, const QList <int> &intervals);

        bool operator !=(const MaskData& other) const;

        QString name;
        DataContainer data;
    };
}

Q_DECLARE_METATYPE(ssu::MaskData)

#endif // MASKDATA_H
