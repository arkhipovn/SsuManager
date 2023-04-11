#include "satellitelistmodel.h"
#include <QDebug>
#include "ssu.h"

namespace ssu {
    SatelliteListModel::SatelliteListModel(QObject *parent) : QAbstractTableModel(parent)
    {

    }

    void SatelliteListModel::set(const QList<Satellite> &list)
    {
        clear();
        beginInsertRows(QModelIndex(), 0, list.size() - 1);
        list_ = list;
        endInsertRows();
    }

    void SatelliteListModel::clear()
    {
        if(!list_.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, list_.size() - 1);
            list_.clear();
            endRemoveRows();
        }
    }

    QVariant SatelliteListModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Horizontal) {
            switch (section) {
                case Columns::SatId:
                    return tr("ID");
                case Columns::SatType:
                    return tr("Type");
                case Columns::Elevation:
                    return tr("Elevation, °");
                case Columns::Azimuth:
                    return tr("Azimuth, °");
                case Columns::Snr:
                    return tr("SNR, dB-Hz");
                case Columns::SatTracked:
                    return tr("Tracked");
                default:
                    break;
            }
        }
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    int SatelliteListModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return list_.size();
    }

    int SatelliteListModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return Columns::SatTracked + 1; // 6
    }

    QVariant SatelliteListModel::data(const QModelIndex &index, int role) const
    {
        if(!index.isValid())
            return QVariant();

        switch (role) {
            case Qt::DisplayRole: {
                switch (index.column()) {
                    case Columns::SatId:
                        return list_.at(index.row()).id;
                    case Columns::SatType:
                        return SSU::satelliteSystemToString(static_cast<SSU::SatelliteSystem>(list_.at(index.row()).type));
                    case Columns::Elevation:
                        return list_.at(index.row()).elevation;
                    case Columns::Azimuth:
                        return list_.at(index.row()).azimuth;
                    case Columns::Snr:
                        return list_.at(index.row()).snr;
                    case Columns::SatTracked:
                        return list_.at(index.row()).tracked;
                    default:
                        break;
                }
            }
            case Role::TypeRole:
                return list_.at(index.row()).type;
            case Role::IdRole:
                return list_.at(index.row()).id;
            case Role::ElevationRole:
                return list_.at(index.row()).elevation;
            case Role::AzimuthRole:
                return list_.at(index.row()).azimuth;
            case Role::SnrRole:
                return list_.at(index.row()).snr;
            case Role::TrackedRole:
                return list_.at(index.row()).tracked;
            default:
                break;
        }

        return QVariant();
    }

    QHash<int, QByteArray> SatelliteListModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Qt::DisplayRole, "display" },
            { Role::TypeRole, "type" },
            { Role::IdRole, "satId" },
            { Role::ElevationRole, "elevation" },
            { Role::AzimuthRole, "azimuth" },
            { Role::SnrRole, "snr" },
            { Role::TrackedRole, "tracked" }
        };
        return roles;
    }
}
