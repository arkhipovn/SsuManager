#include "concatenatemasklistmodel.h"
#include "abstractmasklistmodel.h"

namespace ssu {
    ConcatenateMaskListModel::ConcatenateMaskListModel(QObject *parent) : QConcatenateTablesProxyModel(parent)
    {
        addSourceModel(new EmptyMaskModel(this));
    }

    QHash<int, QByteArray> ConcatenateMaskListModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { AbstractMaskListModel::NameRole, "name" },
            { AbstractMaskListModel::Role::MaskRole, "mask" },
            { AbstractMaskListModel::Role::MaskDataRole, "maskData" },
        };
        return roles;
    }

    MaskData ConcatenateMaskListModel::mask(int index) const
    {
        if(index < 0 || index >= rowCount())
            return MaskData("Invalid");
        return qvariant_cast<MaskData>(data(this->index(index, 0), AbstractMaskListModel::Role::MaskRole));
    }
}
