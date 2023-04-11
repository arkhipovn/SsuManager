#include "eventlogmodel.h"
#include "vch003/vch003.h"
#include "vch003/database/vch003tables.h"

namespace ssu {
    EventLogModel::EventLogModel(QObject *parent) : AbstractEventLogModel(static_cast<int>(Vch003::DatabaseTable::EventLog), parent),
        startTime_(0),
        endTime_(0)
    {

    }

    void EventLogModel::setInterval(const QDateTime &start, const QDateTime &end)
    {
        qint64 startSec = start.isValid() ? start.toSecsSinceEpoch()
                                          : 0;
        qint64 endSec = end.isValid() ? end.toSecsSinceEpoch()
                                      : 0;
        if(startTime_ != startSec || endTime_ != endSec) {
            startTime_ = startSec;
            endTime_ = endSec;
            update(); // обновим..
        }
    }

    void EventLogModel::update()
    {
        if(device_ == nullptr)
            return;

        qDebug() << "EventLogModel::update() start";
        auto table = static_cast<Vch003EventTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            QStringList filters;
            if(startTime_ != 0) {
                filters.append(Vch003UserActivityLogTable::columnName(Vch003UserActivityLogTable::Columns::OccurredTime) +
                               " >= " +
                               QString::number(startTime_));
            }
            if(endTime_ != 0) {
                filters.append(Vch003UserActivityLogTable::columnName(Vch003UserActivityLogTable::Columns::OccurredTime) +
                               " <= " +
                               QString::number(endTime_));
            }

            auto list = table->select(device_->database(),
                                      filters);
            set(list);
        }

        qDebug() << "EventLogModel::update() end";
    }
}
