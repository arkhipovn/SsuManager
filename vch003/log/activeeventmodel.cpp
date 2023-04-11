#include "activeeventmodel.h"
#include "vch003/vch003.h"
#include "vch003/database/vch003tables.h"

namespace ssu {
    ActiveEventModel::ActiveEventModel(QObject *parent) : AbstractEventLogModel(static_cast<int>(Vch003::DatabaseTable::ActiveEvents), parent),
        numberOfCriticalEvents_(0),
        numberOfMinorEvents_(0),
        numberOfMajorEvents_(0),
        numberOfWarningEvents_(0)
    {

    }

    bool ActiveEventModel::setDevice(Device *device)
    {
        if(AbstractEventLogModel::setDevice(device)) {
            if(device_ != nullptr) {
                // Нужны еще зависимости от других таблиц
                auto table = device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::HiddenEvents));
                if(table != nullptr) {
                    QObject::connect(table, &Table::dataChanged, this, &AbstractDeviceLogModel::update);
                }
            }
            return true;
        }
        return false;
    }

    bool ActiveEventModel::hideEvents(const QModelIndexList &indexes)
    {
        if(device_ == nullptr)
            return false;
        auto table = static_cast<Vch003HiddenEventsTable*>(device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::HiddenEvents)));
        if(table != nullptr) {
            QList <Vch003Event> events;
            for(const auto &index : indexes) {
                if(index.row() > rowCount())
                    continue;
                Vch003Event event = model_.at(index.row());
                event.clearedTime = 0;
                event.occurredTime = 0;
                event.hash = 0;
                event.comment = "";
                events.append(event);
            }

            int count = table->insert(device_->database(), events);
            return count > 0;
        }
        return false;
    }

    bool ActiveEventModel::contains(const QString &slot, int eventId) const
    {
        for(int i = 0; i < rowCount(); ++i) {
            if(model_.at(i).slot.compare(slot, Qt::CaseInsensitive) != 0)
                continue;
            if(model_.at(i).eventId != eventId)
                continue;
            return true;
        }
        return false;
    }

    void ActiveEventModel::update()
    {
        if(device_ == nullptr)
            return;

        qDebug() << "ActiveEventModel::update() start";
        auto table = static_cast<Vch003EventTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            auto hiddenTable = static_cast<Vch003HiddenEventsTable*>(device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::HiddenEvents)));

            QList <Vch003Event> events;
            if(hiddenTable == nullptr) {
                events = table->select(device_->database(), {});
            }
            else {
                hiddenTable->create(device_->database());   //@N Проверить существует ли таблица hiddenTable
                // Если не проверить, то следующий запрос не выполнится нормально..

                //@N А мб самому пройтись? Без запроса в базу..?

                events = table->select(device_->database(), {
                                           "NOT EXISTS "
                                           "(SELECT 1 FROM '" + hiddenTable->name() + "' AS H "
                                           "WHERE "
                                           "(H.Slot IS NULL OR '" + table->name() + "'.Slot = H.Slot) "
                                           "AND "
                                           "(H.CardType IS NULL OR H.CardType = -1 OR '" + table->name() + "'.CardType = H.CardType) "
                                           "AND "
                                           "(H.EventId IS NULL OR H.EventId = -1 OR '" + table->name() + "'.EventId = H.EventId) "
                                           "AND "
                                           "(H.Channel IS NULL OR H.Channel = -1 OR '" + table->name() + "'.Channel = H.Channel) "
                                           "AND "
                                           "(H.Parameters IS NULL OR '" + table->name() + "'.Parameters = H.Parameters) "
                                           ")"
                                       });
            }

            set(events);
            updateNumberOfStatusEvents();
        }

        qDebug() << "ActiveEventModel::update() end";
    }

    void ActiveEventModel::updateStatusEvents()
    {
        AbstractEventLogModel::updateStatusEvents();
        updateNumberOfStatusEvents();
    }

    void ActiveEventModel::updateNumberOfStatusEvents()
    {
        if(device_ != nullptr) {
            numberOfCriticalEvents_ = 0;
            numberOfMinorEvents_ = 0;
            numberOfMajorEvents_ = 0;
            numberOfWarningEvents_ = 0;

            for(int i = 0; i < rowCount(); ++i) {
                auto it = device_->eventDescriptionList()->constFind(static_cast<int>(model_.at(i).card),
                                                                     model_.at(i).eventId);
                if(it != device_->eventDescriptionList()->constEnd()) {
                    switch (it->status()) {
                        case SSU::EventStatus::Critical:
                            ++numberOfCriticalEvents_;
                            break;
                        case SSU::EventStatus::Major:
                            ++numberOfMajorEvents_;
                            break;
                        case SSU::EventStatus::Minor:
                            ++numberOfMinorEvents_;
                            break;
                        case SSU::EventStatus::Warning:
                            ++numberOfWarningEvents_;
                            break;
                        default:
                            break;
                    }
                }
            }

            emit eventsChanged();
        }
    }

    int ActiveEventModel::numberOfCriticalEvents() const
    {
        return numberOfCriticalEvents_;
    }

    int ActiveEventModel::numberOfMinorEvents() const
    {
        return numberOfMinorEvents_;
    }

    int ActiveEventModel::numberOfMajorEvents() const
    {
        return numberOfMajorEvents_;
    }

    int ActiveEventModel::numberOfWarningEvents() const
    {
        return numberOfWarningEvents_;
    }

    void ActiveEventModel::clear()
    {
        AbstractEventLogModel::clear();

        numberOfCriticalEvents_ = 0;
        numberOfMinorEvents_ = 0;
        numberOfMajorEvents_ = 0;
        numberOfWarningEvents_ = 0;
    }

}
