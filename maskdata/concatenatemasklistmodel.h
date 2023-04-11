#ifndef CONCATENATEMASKLISTMODEL_H
#define CONCATENATEMASKLISTMODEL_H

#include <QConcatenateTablesProxyModel>
#include "maskdata.h"

namespace ssu {
    class ConcatenateMaskListModel : public QConcatenateTablesProxyModel
    {
        Q_OBJECT

    public:
        explicit ConcatenateMaskListModel(QObject *parent = nullptr);
        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE MaskData mask(int index) const;
    };
}

#endif // CONCATENATEMASKLISTMODEL_H
