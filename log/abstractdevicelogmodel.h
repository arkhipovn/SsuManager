#ifndef ABSTRACTDEVICELOGMODEL_H
#define ABSTRACTDEVICELOGMODEL_H

#include <QAbstractTableModel>
#include "database/table.h"
#include "device/device.h"

namespace ssu {
    class AbstractDeviceLogModel : public QAbstractTableModel
    {
        Q_OBJECT

        Q_PROPERTY(ssu::Device *device READ device WRITE setDevice NOTIFY deviceChanged)

    public:
        explicit AbstractDeviceLogModel(int tableId, QObject *parent = nullptr);
        virtual ~AbstractDeviceLogModel() = default;

        Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        inline Device *device() const { return device_; }
        virtual bool setDevice(Device *device);

    public slots:
        virtual void update() = 0;

    signals:
        void deviceChanged();

    protected:
        int tableId_;
        Device *device_;
        QStringList horizontalHeader_;
    };
}

#endif // ABSTRACTDEVICELOGMODEL_H

