#include "maskdata.h"
#include "ssu.h"

ssu::MaskData::MaskData(const QString &name) : name(name)
{

}

ssu::MaskData::MaskData(const QString &name, const ssu::DataContainer &data) : name(name), data(data)
{

}

ssu::MaskData::MaskData(MaskRecommendation *recommendation, const QList<int> &intervals)
{
    if(recommendation != nullptr) {
        name = recommendation->name();
        data.set(static_cast<int>(SSU::MeasDataType::Mtie), recommendation->mtie(intervals));
        data.set(static_cast<int>(SSU::MeasDataType::Tdev), recommendation->tdev(intervals));
    }
}

bool ssu::MaskData::operator !=(const MaskData &other) const
{
    return name != other.name;
}





