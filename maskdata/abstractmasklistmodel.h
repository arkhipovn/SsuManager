#ifndef ABSTRACTMEASKLISTMODEL_H
#define ABSTRACTMEASKLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "maskdata.h"

namespace ssu {
    class AbstractMaskListModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        explicit AbstractMaskListModel(QObject *parent = nullptr);
        virtual ~AbstractMaskListModel() = default;

        QHash<int, QByteArray> roleNames() const override;
        QVariant data(const QModelIndex &index, int role) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        Q_INVOKABLE virtual MaskData mask(int index) const;
        Q_INVOKABLE virtual MaskData findMask(const QString &name) const;

        enum Role {
            NameRole = Qt::UserRole + 1,
            MaskRole,
            MaskDataRole
        };
    };

    using EmptyMaskModel = AbstractMaskListModel;
}

#endif // ABSTRACTMEASKLISTMODEL_H
