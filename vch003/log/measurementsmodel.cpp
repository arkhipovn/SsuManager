#include "measurementsmodel.h"

namespace ssu {
    MeasurementsModel::MeasurementsModel(QObject *parent) : CurrentMeasurementsModel(parent)
    {

    }

    void MeasurementsModel::set(const QList<MeasData> &list)
    {
        AbstractMeasLogModel::set(list);
    }

    void MeasurementsModel::append(const MeasData &data)
    {
        beginInsertRows(QModelIndex(), model_.size(), model_.size());
        model_.append(data);
        endInsertRows();
    }

    bool MeasurementsModel::setDevice(Device *device)
    {
        if(device_ != device) {
            device_ = device;
            emit deviceChanged();
            return true;
        }
        return false;
    }

    void MeasurementsModel::update()
    {

    }
}
