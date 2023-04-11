#ifndef SATELLITELISTMODEL_H
#define SATELLITELISTMODEL_H

#include <QAbstractTableModel>
#include "satellite.h"

namespace ssu {
    class SatelliteListModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        explicit SatelliteListModel(QObject *parent = nullptr);

        void set(const QList <Satellite> &list);
        void clear();

        Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation,
                                        int role = Qt::DisplayRole) const override;

        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash <int, QByteArray> roleNames() const;

        enum Columns {
            SatId,
            SatType,
            Elevation,
            Azimuth,
            Snr,
            SatTracked
        };
        Q_ENUMS(Columns)

    private:
        enum Role {
            TypeRole = Qt::UserRole + 1,
            IdRole,
            ElevationRole,
            AzimuthRole,
            SnrRole,
            TrackedRole
        };

        QList <Satellite> list_;
    };
}

#endif // SATELLITELISTMODEL_H
