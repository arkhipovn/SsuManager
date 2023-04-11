#ifndef EVENTDESCRIPTIONFILTERPROXYMODEL_H
#define EVENTDESCRIPTIONFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace ssu {
    // А точно нужно? Мб использовать просто QSortFilterProxyModel и указать filterRegExp?

    class EventDescriptionFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

        Q_PROPERTY(int cardType READ cardType WRITE setCardType NOTIFY cardTypeChanged)

    public:
        explicit EventDescriptionFilterProxyModel(QObject *parent = nullptr);

        int cardType() const;
        void setCardType(int cardType);

    protected:
         bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    signals:
        void cardTypeChanged();

    private:
        int cardType_;
    };
}

#endif // EVENTDESCRIPTIONFILTERPROXYMODEL_H
