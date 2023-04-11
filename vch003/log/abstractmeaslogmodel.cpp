#include "abstractmeaslogmodel.h"
#include <math.h>
#include "vch003/database/vch003tables.h"
#include "csettings.h"

namespace ssu {
    AbstractMeasLogModel::AbstractMeasLogModel(int tableId, QObject *parent) : AbstractDeviceLogModel(tableId, parent),
        shortHeaderTitles_(true)
    {
        reclist_ = new ReclistModel(this);
        QObject::connect(reclist_, &ReclistModel::dataChanged, this, &AbstractMeasLogModel::updateModel);
    }

    QHash<int, QByteArray> AbstractMeasLogModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Qt::DisplayRole, "display" },
            { Qt::BackgroundRole, "background" }
        };
        return roles;
    }

    int AbstractMeasLogModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return model_.size();
    }

    void AbstractMeasLogModel::clear()
    {
        if(!model_.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, model_.size() - 1);
            model_.clear();
            endRemoveRows();
        }
    }

    bool AbstractMeasLogModel::useMask() const
    {
        return !mask_.name.isEmpty() || !mask_.data.isNull();
    }

    MaskData AbstractMeasLogModel::mask() const
    {
        return mask_;
    }

    void AbstractMeasLogModel::setMask(const MaskData &mask)
    {
        mask_ = mask;
        emit maskChanged();
        emit dataChanged(index(0, Values), index(rowCount() - 1, columnCount() - 1), { Qt::BackgroundRole });
    }

    void AbstractMeasLogModel::clearMask()
    {
        setMask(MaskData());
    }

    void AbstractMeasLogModel::set(const QList<MeasData> &list)
    {
        clear();
        if(!list.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, list.size() - 1);
            model_ = list;
            endInsertRows();
        }
    }

    void AbstractMeasLogModel::update()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003MeasurementTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            reclist_->update(table->readReclist(device_->database()));
        }
        updateModel();
    }

    void AbstractMeasLogModel::updateModel()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003MeasurementTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            QList <MeasData> list;
            for(int i = 0; i < reclist_->size(); ++i) {
                const auto &rec = reclist_->at(i);
                if(rec.selected) {
                    list.append(table->select(device_->database(), rec.channel));
                }
            }
            set(list);
        }
    }

    bool AbstractMeasLogModel::shortHeaderTitles() const
    {
        return shortHeaderTitles_;
    }

    void AbstractMeasLogModel::setShortHeaderTitles(bool shortHeaderTitles)
    {
        shortHeaderTitles_ = shortHeaderTitles;
    }

    ReclistModel *AbstractMeasLogModel::reclist() const
    {
        return reclist_;
    }

    QList<MeasData> AbstractMeasLogModel::getMeasData(const QModelIndexList &indexes) const
    {
        QList<MeasData> list;
        for(const auto &index : indexes) {
            if(index.row() > rowCount())
                continue;
            list.append(model_.at(index.row()));
        }
        return list;
    }

    QVariant AbstractMeasLogModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Vertical && role == Qt::DisplayRole) {
            if(section >= 0 && section < rowCount()) {
                return shortHeaderTitles_ ? data(index(section, Columns::Channel)).toString()
                                          : data(index(section, Columns::Channel)).toString() + "/" +
                                            data(index(section, Columns::Time)).toDateTime().toString(CSettings::getInstance()->dateTimeFormat());
            }
        }
        return AbstractDeviceLogModel::headerData(section, orientation, role);
    }



}
