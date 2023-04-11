#include "hiddeneventmodel.h"
#include "vch003/vch003.h"
#include "vch003/database/vch003tables.h"

namespace ssu {
    HiddenEventModel::HiddenEventModel(QObject *parent) : AbstractEventLogModel(static_cast<int>(Vch003::DatabaseTable::HiddenEvents), parent)
    {

    }

    bool HiddenEventModel::showEvents(const QModelIndexList &indexes)
    {
        if(model_.isEmpty())
            return true;
        if(indexes.isEmpty())
            return true;
        if(device_ == nullptr)
            return false;

        QList <Vch003Event> showEvents;
        for(const auto &index : indexes) {
            if(index.row() > rowCount())
                continue;
            showEvents.append(model_.at(index.row()));
        }

        auto events = model_;
        for(const auto &event : showEvents) {
            events.removeOne(event);
        }

        auto table = static_cast<Vch003HiddenEventsTable*>(device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::HiddenEvents)));

        bool isok;
        table->drop(device_->database(), &isok);     // Нам нужна пустая таблица. Нужно все переписать..
        if(!isok)
            return false;
        table->insert(device_->database(), events, &isok);
        return isok;
    }

    bool HiddenEventModel::showAllEvents()
    {
        if(model_.isEmpty())
            return true;
        if(device_ == nullptr)
            return false;
        auto table = static_cast<Vch003HiddenEventsTable*>(device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::HiddenEvents)));
        bool isok;
        table->drop(device_->database(), &isok);     // Нам нужна пустая таблица. Нужно все переписать..
        return isok;
    }
}
