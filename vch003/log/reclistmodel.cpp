#include "reclistmodel.h"
#include <QDebug>

namespace ssu {
    ReclistModel::ReclistModel(QObject *parent) : QAbstractListModel(parent)
    {

    }

    void ReclistModel::set(const QList<int> &channels)
    {
        clear();
        if(!channels.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, channels.size() - 1);
            for(auto &channel : channels) {
                Record r; r.channel = channel;
                model_.append(r);
            }
            endInsertRows();
        }
    }

    void ReclistModel::set(const QList <ReclistModel::Record> &records)
    {
        clear();
        if(!records.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, records.size() - 1);
            model_ = records;
            endInsertRows();
        }
    }

    void ReclistModel::update(const QList<int> &channels)
    {
        QList <Record> newModel;     // новый список
        bool updated = model_.size() != channels.size();

        for(auto &channel : channels) {
            Record r; r.channel = channel;
            int index = model_.indexOf(r);

            if(index == -1) {
                // что-то точно изменилось..
                updated = true;
            }
            else {
                // сверим
                r.selected = model_.at(index).selected;
            }

            newModel.append(r);
        }

        if(updated) {
            set(newModel);
        }
    }

    void ReclistModel::clear()
    {
        if(!model_.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, model_.size() - 1);
            model_.clear();
            endRemoveRows();
        }
    }

    QHash<int, QByteArray> ReclistModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Role::SelectedRole, "selected" },
            { Role::ChannelRole, "channel" },
        };
        return roles;
    }

    QVariant ReclistModel::data(const QModelIndex &index, int role) const
    {
        QVariant v;
        if(!index.isValid())
            return v;
        if(index.row() >= model_.size())
            return v;
        switch (role) {
            case Role::ChannelRole: return model_.at(index.row()).channel;
            case Role::SelectedRole: return model_.at(index.row()).selected;
            default: break;
        }
        return v;
    }

    bool ReclistModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if(!value.isValid())
            return false;
        if(index.row() >= model_.size())
            return false;
        if(role != Role::SelectedRole)
            return false;
        if(model_.at(index.row()).selected != value.toBool()) {
            model_[index.row()].selected = value.toBool();
            emit dataChanged(index, index, { Role::SelectedRole });
        }
        return true;
    }

    int ReclistModel::rowCount(const QModelIndex &index) const
    {
        Q_UNUSED(index)
        return model_.size();
    }

    bool ReclistModel::isEmpty() const
    {
        return model_.isEmpty();
    }

    int ReclistModel::size() const
    {
        return model_.size();
    }

    void ReclistModel::select(int i, bool value)
    {
        if(model_.at(i).selected != value) {
            model_[i].selected = value;
            emit dataChanged(index(i, 0), index(i, 0), { Role::SelectedRole });
        }
    }

    bool ReclistModel::isSelected(int i) const
    {
        if(i < 0 || i >= model_.size()) return false;
        return model_.at(i).selected;
    }

    void ReclistModel::selectAll(bool value)
    {
        for(int i = 0; i < model_.size(); ++i) {
            model_[i].selected = value;
        }
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0), { Role::SelectedRole });
    }

    const ReclistModel::Record &ReclistModel::at(int index) const
    {
        return model_.at(index);
    }

    bool ReclistModel::Record::operator <(const ReclistModel::Record &other) const
    {
        return channel < other.channel;
    }

    bool ReclistModel::Record::operator ==(int channel) const
    {
        return this->channel == channel;
    }

    bool ReclistModel::Record::operator ==(const ReclistModel::Record &other) const
    {
        return this->channel == other.channel;
    }

}
