#include "eventdescriptionfilterproxymodel.h"
#include "eventdescriptionlistmodel.h"
#include <QDebug>

namespace ssu {
    EventDescriptionFilterProxyModel::EventDescriptionFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent),
        cardType_(SSU::CardType::InvalidType)
    {
        QSortFilterProxyModel::setFilterRole(EventDescriptionListModel::Role::CardTypeRole);
    }
    
    int EventDescriptionFilterProxyModel::cardType() const
    {
        return cardType_;
    }
    
    void EventDescriptionFilterProxyModel::setCardType(int cardType)
    {
        if(cardType_ != cardType) {
            cardType_ = cardType;
            invalidateFilter();
            emit cardTypeChanged();
        }
    }

    bool EventDescriptionFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        if(sourceModel() == nullptr)
            return true;
        auto sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
        if(!sourceIndex.isValid()) return true;
        return sourceModel()->data(sourceIndex, filterRole()).toInt() == cardType_;
    }
}
