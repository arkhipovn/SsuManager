#include "eventdescriptionlistmodel.h"
#include <QDebug>

namespace ssu {
    EventDescriptionListModel::EventDescriptionListModel(QObject *parent) : QAbstractListModel(parent)
    {

    }

    bool EventDescriptionListModel::appendEvent(const EventDescription &event)
    {
        auto it = std::lower_bound(model_.begin(), model_.end(), event);
        if(it != model_.constEnd() && !(event < (*it))) // уже есть..
            return false;
        model_.insert(it, event);
        return true;
    }

    void EventDescriptionListModel::append(const EventDescription &event)
    {
        if(appendEvent(event))
            emit numberOfEventsChanged();
    }

    void EventDescriptionListModel::append(const QList<EventDescription> &events)
    {
        bool changed = false;
        for(auto &event : events) {
            if(appendEvent(event)) {
                changed = true;
            }
        }

        if(changed)
            emit numberOfEventsChanged();
    }

    void EventDescriptionListModel::update(const QList<EventDescription> &events)
    {
        bool eventStatusChanged = false;
        bool countChanged = false;

        for(auto &event : events) {
            auto it = std::lower_bound(model_.begin(), model_.end(), event);
            if(it != model_.constEnd() && !(event < (*it))) { // уже есть..
                if(it->status() != event.status()) {
                    it->setStatus(event.status());
                    eventStatusChanged = true;
                }
            }
            else {
                model_.insert(it, event);
                countChanged = true;
            }
        }

        if(eventStatusChanged)
            emit statusChanged();

        if(countChanged)
            emit numberOfEventsChanged();
    }

    QHash<int, QByteArray> EventDescriptionListModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Role::IdRole, "eventId" },
            { Role::CardTypeRole, "cardType" },
            { Role::DescriptionRole, "description" },
            { Role::StatusRole, "eventStatus" }
        };
        return roles;
    }

    QVariant EventDescriptionListModel::data(const QModelIndex &index, int role) const
    {
        if(index.row() < 0 || index.row() >= model_.size())
            return QVariant();

        switch (role) {
            case Role::IdRole:
                return model_.at(index.row()).id();
            case Role::CardTypeRole:
                return model_.at(index.row()).cardType();
            case Role::DescriptionRole:
                return model_.at(index.row()).description();
            case Role::StatusRole:
                return static_cast<int>(model_.at(index.row()).status());
            default:
                break;
        }

        return QVariant();
    }

    int EventDescriptionListModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return size();
    }

    QStringList EventDescriptionListModel::descriptions() const
    {
        QSet <QString> descriptions;
        for(auto &m : model_) {
            if(!descriptions.contains(m.description())) {
                descriptions.insert(m.description());
            }
        }
        return descriptions.values();
    }

    const EventDescription &EventDescriptionListModel::operator[](int i) const
    {
        return model_[i];
    }

    const EventDescription &EventDescriptionListModel::at(int i) const
    {
        return model_.at(i);
    }

    bool EventDescriptionListModel::isEmpty() const
    {
        return model_.isEmpty();
    }

    int EventDescriptionListModel::size() const
    {
        return model_.size();
    }

    EventDescriptionListModel::const_iterator EventDescriptionListModel::constBegin() const
    {
        return model_.constBegin();
    }

    EventDescriptionListModel::const_iterator EventDescriptionListModel::constEnd() const
    {
        return model_.constEnd();
    }

    EventDescriptionListModel::iterator EventDescriptionListModel::find(int cardType, int id)
    {
        EventDescription d(cardType, id);
        auto it = std::lower_bound(model_.begin(), model_.end(), d);
        if(it != model_.end() && !(d < (*it))) return it;   // уже есть..
        return model_.end();
    }

    EventDescriptionListModel::const_iterator EventDescriptionListModel::constFind(int cardType, int id) const
    {
        EventDescription d(cardType, id);
        auto it = std::lower_bound(model_.constBegin(), model_.constEnd(), d);
        if(it != model_.constEnd() && !(d < (*it))) return it;   // уже есть..
        return model_.constEnd();
    }

    void EventDescriptionListModel::updateStatus(int cardType, int id, SSU::EventStatus status)
    {
        auto it = find(cardType, id);
        if(it == end())
            return;
        if(it->status() == status)
            return;
        it->setStatus(status);
        emit statusChanged();
    }

    SSU::EventStatus EventDescriptionListModel::eventStatus(int cardType, int id) const
    {
        auto it = constFind(cardType, id);
        return it == constEnd() ? SSU::EventStatus::NotAlarm
                                : it->status();
    }

    QString EventDescriptionListModel::eventDescription(int cardType, int id) const
    {
        auto it = constFind(cardType, id);
        return it == constEnd() ? ""
                                : it->description();
    }

    EventDescriptionListModel::iterator EventDescriptionListModel::begin()
    {
        return model_.begin();
    }

    EventDescriptionListModel::iterator EventDescriptionListModel::end()
    {
        return model_.end();
    }
}
