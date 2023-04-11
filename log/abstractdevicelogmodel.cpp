#include "abstractdevicelogmodel.h"

namespace ssu {

    AbstractDeviceLogModel::AbstractDeviceLogModel(int tableId, QObject *parent) : QAbstractTableModel(parent),
        tableId_(tableId),
        device_(nullptr)
    {

    }

    QVariant AbstractDeviceLogModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            if(section >= 0 && section < horizontalHeader_.size())
                return horizontalHeader_.at(section);
        }
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    bool AbstractDeviceLogModel::setDevice(Device *device)
    {
        if(device_ != device) {
            device_ = device;
            emit deviceChanged();

            if(device_ != nullptr) {
                auto table = device_->databaseExecutor()->table(tableId_);
                if(table != nullptr) {
                    QObject::connect(table, &Table::dataChanged, this, &AbstractDeviceLogModel::update);
                }
            }
            return true;
        }
        return false;
    }
}


