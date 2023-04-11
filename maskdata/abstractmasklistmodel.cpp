#include "abstractmasklistmodel.h"
#include <QDebug>

namespace ssu {
    AbstractMaskListModel::AbstractMaskListModel(QObject *parent) : QAbstractListModel(parent)
    {

    }

    QHash<int, QByteArray> AbstractMaskListModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Role::NameRole, "name" },
            { Role::MaskRole, "mask" },
            { Role::MaskDataRole, "maskData" },
        };
        return roles;
    }

    QVariant AbstractMaskListModel::data(const QModelIndex &index, int role) const
    {
        if(!index.isValid())
            return QVariant();
        switch (role) {
            case Role::NameRole:
                return tr("Without mask");
            case Role::MaskRole:
                return QVariant::fromValue(MaskData());
            case Role::MaskDataRole:
                return QVariant::fromValue(DataContainer());
            default:
                break;
        }
        return QVariant();
    }

    int AbstractMaskListModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return 1;
    }

    MaskData AbstractMaskListModel::mask(int index) const
    {
        Q_UNUSED(index)
        return MaskData();
    }

    MaskData AbstractMaskListModel::findMask(const QString &name) const
    {
        Q_UNUSED(name)
        return MaskData();
    }

}
