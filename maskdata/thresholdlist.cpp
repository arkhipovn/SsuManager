#include "thresholdlist.h"
#include <QDebug>

namespace ssu {
    ThresholdList::ThresholdList(QObject *parent) : AbstractMaskListModel(parent)
    {

    }

    QVariant ThresholdList::data(const QModelIndex &index, int role) const
    {
        if(!index.isValid())
            return QVariant();
        switch (role) {
            case Role::NameRole:
                return maskList_.at(index.row()).name;
            case Role::MaskRole:
                return QVariant::fromValue(maskList_.at(index.row()));
            case Role::MaskDataRole:
                return QVariant::fromValue(maskList_.at(index.row()).data);
            default:
                break;
        }
        return QVariant();
    }

    int ThresholdList::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return maskList_.size();
    }

    void ThresholdList::append(const MaskData &mask)
    {
        beginInsertRows(QModelIndex(), maskList_.size(), maskList_.size());
        maskList_.append(mask);
        endInsertRows();
    }

    void ThresholdList::setName(int index, const QString &name)
    {
        if(index < 0 || index >= maskList_.size())
            return;
        if(maskList_.at(index).name == name)
            return;
        maskList_[index].name = name;
        emit dataChanged(this->index(index, 0), this->index(index, 0), { Role::NameRole, Role::MaskRole } );
    }

    void ThresholdList::setMaskData(int index, const DataContainer &data)
    {
        if(index < 0 || index >= maskList_.size())
            return;
        maskList_[index].data = data;
        emit dataChanged(this->index(index, 0), this->index(index, 0), { Role::MaskDataRole, Role::MaskRole } );
    }

    MaskData ThresholdList::mask(int index) const
    {
        if(index < 0 || index >= maskList_.size())
            return MaskData("Invalid");
        return maskList_.at(index);
    }

    MaskData ThresholdList::findMask(const QString &name) const
    {
        for(const auto &mask: maskList_) {
            if(mask.name == name)
                return mask;
        }
        return MaskData("Invalid");
    }

    void ThresholdList::clear()
    {
        if(maskList_.isEmpty())
            return;
        beginRemoveRows(QModelIndex(), 0, maskList_.size() - 1);
        maskList_.clear();
        endRemoveRows();
    }
}


