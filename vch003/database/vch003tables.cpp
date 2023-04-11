#include "vch003tables.h"
#include "vch003/vch003.h"
#include "vch003/configuration/vch003options.h"
#include "math.h"
#include <QSqlError>

namespace ssu {
#define EVENT_LOG_TABLE             "EventLog"
#define ACTIVE_EVENTS_TABLE         "ActiveEvents"
#define HIDDEN_EVENTS_TABLE         "HiddenEvents"
#define USER_ACTIVITY_TABLE         "UserActivityLog"
#define MEASUREMENT_LOG_TABLE       "MeasLog"
#define CURRENT_MEASUREMENT_TABLE   "CurrentMeas"

    Vch003EventTable::Vch003EventTable(const QString &name, QObject *parent) : Table(name, parent)
    {

    }

    void Vch003EventTable::create(Database * const db, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        if(name_.isEmpty())
            return;
        if(!db->create())        // проверим есть ли файл базы данных..
            return;
        if(!db->isOpen())
            return;
        QSqlQuery query({
                            "CREATE TABLE IF NOT EXISTS '" + name_ + "' "
                            "(" + columnName(Columns::Hash)                 + " UNSIGNED BIG INT NOT NULL,"
                            " " + columnName(Columns::OccurredTime)         + " BIGINT,"
                            " " + columnName(Columns::ClearedTime)          + " BIGINT,"
                            " " + columnName(Columns::Slot)                 + " VARCHAR(5),"
                            " " + columnName(Columns::CardType)             + " INTEGER,"
                            " " + columnName(Columns::EventId)              + " INTEGER,"
                            " " + columnName(Columns::Channel)              + " INTEGER,"
                            " " + columnName(Columns::Parameters)           + " TEXT,"
                            " " + columnName(Columns::Comment)              + " TEXT,"
                            " " + columnName(Columns::NotificationStatus)   + " INTEGER,"
                            " PRIMARY KEY(Hash))"
                        },
                        *db);
        if(query.exec()) {
            if(ok != nullptr) *ok = true;
        }
    }

    QVariant Vch003EventTable::lastValue(const Database * const db, const QStringList &filters, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        if(name_.isEmpty())
            return QVariant();
        if(db == nullptr)
            return QVariant();
        if(!db->isCreated() || !db->isOpen())
            return QVariant();
        QSqlQuery query(*db);

        if(!query.exec("SELECT " + columnName(Columns::OccurredTime) +
                       " FROM '" + name_ +
                       where(filters) +
                       "' ORDER BY " + columnName(Columns::OccurredTime) +
                       " DESC LIMIT 1"))

            return QVariant();
        if(query.next()) {
            if(ok != nullptr) *ok = true;
            return query.value(0);
        }
        return QVariant();
    }

    int Vch003EventTable::insert(Database * const db, const QVariantList &list, bool *ok)
    {
        QList<Vch003Event> events;
        for(auto &item : list) {
            Vch003Event event = qvariant_cast<Vch003Event>(item);
            events.append(event);
        }
        return insert(db, events, ok);
    }

    int Vch003EventTable::insert(Database * const db, const QList<Vch003Event> &events, bool *ok)
    {
        int count = 0;
        if(ok != nullptr) *ok = false;

        if(events.isEmpty()) {
            if(ok != nullptr) *ok = true;
            return count;
        }

        if(name_.isEmpty())
            return count;
        if(!db->create())        // проверим есть ли файл базы данных..
            return count;
        if(!db->isOpen())
            return count;

        QSqlQuery query(*db);
        bool isok = db->transaction();

        if(!isok)
            return count;

        create(db, &isok);      // проверим есть ли таблица

        if(!isok)
            return count;

        query.prepare({ "INSERT INTO '" + name_ + "'"
                        " VALUES(?,?,?,?,?,?,?,?,?,?)"
                        " ON CONFLICT(" + Vch003EventTable::columnName(Columns::Hash) + ") DO UPDATE SET"
                        " " + Vch003EventTable::columnName(Columns::ClearedTime) + " = EXCLUDED." + Vch003EventTable::columnName(Columns::ClearedTime)
                      });

        for(const auto &event : events) {
            query.addBindValue(qHash(event));
            query.addBindValue(event.occurredTime);
            query.addBindValue(event.clearedTime);
            query.addBindValue(event.slot);
            query.addBindValue(event.card);
            query.addBindValue(event.eventId);
            query.addBindValue(event.channel);
            query.addBindValue(event.parameters);
            query.addBindValue(event.comment);
            query.addBindValue(event.readed);
            if(!query.exec()) {
                db->rollback();
                return 0;
            }
            count += query.numRowsAffected();
        }

        db->commit();

        if(count > 0) {
            emit dataChanged();
        }

        if(ok != nullptr) *ok = true;
        return count;
    }

    QList<Vch003Event> Vch003EventTable::select(Database * const db, const QStringList &filters, bool *ok) const
    {
        QList<Vch003Event> events;
        bool isok;
        QSqlQuery query = Table::select(db, filters, &isok);

        if(ok != nullptr) *ok = isok;

        if(!isok)
            return events;

        while(query.next()) {
            Vch003Event event;

            event.hash = query.value(static_cast<int>(Columns::Hash)).toUInt(&isok);
            if(!isok) continue;

            event.occurredTime = query.value(static_cast<int>(Columns::OccurredTime)).toLongLong(&isok);
            if(!isok) continue;

            event.clearedTime = query.value(static_cast<int>(Columns::ClearedTime)).toLongLong(&isok);
            if(!isok) continue;

            event.slot = query.value(static_cast<int>(Columns::Slot)).toString();

            event.card = static_cast<SSU::CardType>(query.value(static_cast<int>(Columns::CardType)).toInt(&isok));
            if(!isok) continue;

            event.eventId = query.value(static_cast<int>(Columns::EventId)).toInt(&isok);
            if(!isok) continue;

            event.channel = query.value(static_cast<int>(Columns::Channel)).toInt(&isok);
            if(!isok) continue;

            event.parameters = query.value(static_cast<int>(Columns::Parameters)).toString();
            event.comment = query.value(static_cast<int>(Columns::Comment)).toString();

            event.readed = query.value(static_cast<int>(Columns::NotificationStatus)).toBool();

            events.append(event);
        }

        return events;
    }

    QString Vch003EventTable::columnName(Vch003EventTable::Columns column)
    {
        switch (column) {
            case Columns::Hash:                 return "Hash";
            case Columns::OccurredTime:         return "OccurredTime";
            case Columns::ClearedTime:          return "ClearedTime";
            case Columns::Slot:                 return "Slot";
            case Columns::CardType:             return "CardType";
            case Columns::EventId:              return "EventId";
            case Columns::Channel:              return "Channel";
            case Columns::Parameters:           return "Parameters";
            case Columns::Comment:              return "Comment";
            case Columns::NotificationStatus:   return "NotificationStatus";
            default:
                break;
        }
        return "";
    }

    Vch003EventLogTable::Vch003EventLogTable(QObject *parent) : Vch003EventTable(EVENT_LOG_TABLE, parent) {}

    Vch003ActiveEventsTable::Vch003ActiveEventsTable(QObject *parent) : Vch003EventTable(ACTIVE_EVENTS_TABLE, parent) {}

    Vch003HiddenEventsTable::Vch003HiddenEventsTable(QObject *parent) : Vch003EventTable(HIDDEN_EVENTS_TABLE, parent) {}



    // __________________________

    Vch003UserActivityLogTable::Vch003UserActivityLogTable(QObject *parent) : Table(USER_ACTIVITY_TABLE, parent)
    {

    }

    void Vch003UserActivityLogTable::create(Database * const db, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        if(name_.isEmpty())
            return;
        if(db == nullptr)
            return;
        if(!db->create())        // проверим есть ли файл базы данных..
            return;
        if(!db->isOpen())
            return;
        QSqlQuery query({
                            "CREATE TABLE IF NOT EXISTS '" + name_ + "' "
                            "(" + columnName(Columns::Hash)                 + " UNSIGNED BIG INT NOT NULL,"
                            " " + columnName(Columns::OccurredTime)         + " BIGINT,"
                            " " + columnName(Columns::Address)              + " TEXT,"
                            " " + columnName(Columns::Username)             + " TEXT,"
                            " " + columnName(Columns::Action)               + " TEXT,"
                            " " + columnName(Columns::NotificationStatus)   + " INTEGER,"
                            " PRIMARY KEY(Hash))"
                        },
                        *db);

        if(query.exec()) {
            if(ok != nullptr) *ok = true;
        }
    }

    QVariant Vch003UserActivityLogTable::lastValue(const Database * const db, const QStringList &filters, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        if(name_.isEmpty()) {
            return QVariant();
        }
        if(db == nullptr) {
            return QVariant();
        }

        if(!db->isCreated() || !db->isOpen()) {
            return QVariant();
        }

        QString queryStr = "SELECT " + columnName(Columns::OccurredTime) +
                " FROM '" + name_ +
                where(filters) +
                "' ORDER BY " + columnName(Columns::OccurredTime) +
                " DESC LIMIT 1";

        QSqlQuery query(queryStr,
                        db->database());

        if(!query.exec()) {
            return QVariant();
        }

        if(query.next()) {
            if(ok != nullptr) *ok = true;
            return query.value(0);
        }

        return QVariant();
    }

    int Vch003UserActivityLogTable::insert(Database * const db, const QVariantList &list, bool *ok)
    {
        QList<Vch003UserActivity> activities;
        for(auto &item : list) {
            Vch003UserActivity activity = qvariant_cast<Vch003UserActivity>(item);
            activities.append(activity);
        }
        return insert(db, activities, ok);
    }

    int Vch003UserActivityLogTable::insert(Database * const db, const QList<Vch003UserActivity> &activities, bool *ok)
    {
        int count = 0;
        if(ok != nullptr) *ok = false;
        if(db == nullptr)
            return count;
        if(activities.isEmpty()) {
            if(ok != nullptr) *ok = true;
            return count;
        }

        if(name_.isEmpty())
            return count;
        if(!db->create())        // проверим есть ли файл базы данных..
            return count;
        if(!db->isOpen())
            return count;

        QSqlQuery query(*db);
        bool isok = db->transaction();

        if(!isok)
            return count;

        create(db, &isok);      // проверим есть ли таблица

        if(!isok)
            return count;

        query.prepare({ "INSERT OR IGNORE INTO '" + name_ + "'"
                        " VALUES(?,?,?,?,?,?)"
                      });

        for(const auto &activity : activities) {
            query.addBindValue(qHash(activity));
            query.addBindValue(activity.occurredTime);
            query.addBindValue(activity.address);
            query.addBindValue(activity.username);
            query.addBindValue(activity.action);
            query.addBindValue("");
            if(!query.exec()) {
                db->rollback();
                return 0;
            }
            count += query.numRowsAffected();
        }

        db->commit();

        if(count > 0) {
            emit dataChanged();
        }

        if(ok != nullptr) *ok = true;
        return count;
    }

    QList<Vch003UserActivity> Vch003UserActivityLogTable::select(Database * const db, const QStringList &filters, bool *ok) const
    {
        QList<Vch003UserActivity> activities;
        bool isok;
        QSqlQuery query = Table::select(db, filters, &isok);

        if(ok != nullptr) *ok = isok;

        if(!isok)
            return activities;

        while(query.next()) {
            Vch003UserActivity activity;
            activity.occurredTime = query.value(static_cast<int>(Columns::OccurredTime)).toLongLong(&isok);
            if(!isok) continue;
            activity.address = query.value(static_cast<int>(Columns::Address)).toString();
            activity.action = query.value(static_cast<int>(Columns::Action)).toString();
            activity.username = query.value(static_cast<int>(Columns::Username)).toString();
            activities.append(activity);
        }
        return activities;
    }

    QString Vch003UserActivityLogTable::columnName(Vch003UserActivityLogTable::Columns column)
    {
        switch (column) {
            case Columns::Hash:                 return "Hash";
            case Columns::OccurredTime:         return "OccurredTime";
            case Columns::Address:              return "Address";
            case Columns::Username:             return "Username";
            case Columns::Action:               return "Action";
            case Columns::NotificationStatus:   return "NotificationStatus";
            default:
                break;
        }
        return "";
    }









    //@N measlog
    static const QList <int> measInterval = { 1, 10, 100, 1000, 10000 };
#define NUMBER_OF_CHARACTERISTICS 3

    Vch003MeasurementTable::Vch003MeasurementTable(const QString &name, QObject *parent) : Table(name, parent)
    {

    }

    void Vch003MeasurementTable::create(Database * const db, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        if(name_.isEmpty())
            return;
        if(!db->create())        // проверим есть ли файл базы данных..
            return;
        if(!db->isOpen())
            return;

        QStringList mtieHeader;
        QStringList tdevHeader;
        QStringList rfHeader;
        for(const auto &interval : measInterval) {
            mtieHeader << (columnName(Columns::Mtie) + QString::number(interval) + " REAL");
            tdevHeader << (columnName(Columns::Tdev) + QString::number(interval) + " REAL");
            rfHeader << (columnName(Columns::Rf) + QString::number(interval) + " REAL");
        }

        QSqlQuery query({
                            "CREATE TABLE IF NOT EXISTS '" + name_ + "' "
                            "(" + columnName(Columns::Time)     + " BIGINT,"
                            " " + columnName(Columns::Channel)  + " INTEGER,"
                            " " + mtieHeader.join(", ") +
                            ", " + tdevHeader.join(", ") +
                            ", " + rfHeader.join(", ") +
                            ", UNIQUE(" + columnName(Columns::Time) + ", " + columnName(Columns::Channel) + "))"
                        }, *db);

        if(query.exec()) {
            if(ok != nullptr) *ok = true;
        }
    }

    QVariant Vch003MeasurementTable::lastValue(const Database * const db, const QStringList &filters, bool *ok) const
    {
        Q_UNUSED(db)
        Q_UNUSED(filters)
        Q_UNUSED(ok)

        if(ok != nullptr) *ok = false;
        if(name_.isEmpty())
            return QVariant();
        if(db == nullptr)
            return QVariant();
        if(!db->isCreated() || !db->isOpen())
            return QVariant();
        QSqlQuery query(*db);

        if(!query.exec("SELECT " + columnName(Columns::Time) +
                       " FROM '" + name_ +
                       where(filters) +
                       "' ORDER BY " + columnName(Columns::Time) +
                       " DESC LIMIT 1"))

            return QVariant();
        if(query.next()) {
            if(ok != nullptr) *ok = true;
            return query.value(0);
        }
        return QVariant();
    }

    QVariant Vch003MeasurementTable::lastValue(const Database * const db, int channel, bool *ok) const
    {
        return lastValue(db, { columnName(Columns::Channel) + " = " + QString::number(channel)}, ok);
    }

    int Vch003MeasurementTable::insert(Database * const db, const QVariantList &list, bool *ok)
    {
        QList <MeasData> measurements;
        for(auto &item : list) {
            MeasData meas = qvariant_cast<MeasData>(item);
            measurements.append(meas);
        }
        return insert(db, measurements, ok);
    }

    int Vch003MeasurementTable::insert(Database * const db, const QList<MeasData> &meas, bool *ok)
    {
        int count = 0;
        if(ok != nullptr) *ok = false;
        if(db == nullptr)
            return count;
        if(meas.isEmpty()) {
            if(ok != nullptr) *ok = true;
            return count;
        }

        if(name_.isEmpty())
            return count;
        if(!db->create())        // проверим есть ли файл базы данных..
            return count;
        if(!db->isOpen())
            return count;

        QSqlQuery query(*db);
        bool isok = db->transaction();

        if(!isok)
            return count;

        create(db, &isok);      // проверим есть ли таблица

        if(!isok)
            return count;

        QString temp = QString("?,").repeated(measInterval.size()).repeated(NUMBER_OF_CHARACTERISTICS);
        temp.chop(1);
        query.prepare({ "INSERT OR IGNORE INTO '" + name_ + "'"
                        " VALUES(?,?," + temp + ")"
                      });

        for(const auto &m : meas) {
            query.addBindValue(m.time);
            query.addBindValue(m.channel);

            for(int i = 0; i < measInterval.size(); ++i) {
                double value = m.data.value(static_cast<int>(SSU::MeasDataType::Mtie), i);
                if(qIsNaN(value))
                    query.addBindValue("");
                else
                    query.addBindValue(value);
            }

            for(int i = 0; i < measInterval.size(); ++i) {
                double value = m.data.value(static_cast<int>(SSU::MeasDataType::Tdev), i);
                if(qIsNaN(value))
                    query.addBindValue(QVariant());
                else
                    query.addBindValue(value);
            }

            for(int i = 0; i < measInterval.size(); ++i) {
                double value = m.data.value(static_cast<int>(SSU::MeasDataType::Rf), i);
                if(qIsNaN(value))
                    query.addBindValue(QVariant());
                else
                    query.addBindValue(value);
            }

            if(!query.exec()) {
                db->rollback();
                return 0;
            }
            count += query.numRowsAffected();
        }

        db->commit();

        if(count > 0) {
            emit dataChanged();
        }

        if(ok != nullptr) *ok = true;
        return count;
    }

    QList<MeasData> Vch003MeasurementTable::select(Database * const db, const QStringList &filters, bool *ok) const
    {
        QList<MeasData> meas;
        bool isok;
        QSqlQuery query = Table::select(db, filters, &isok);

        if(ok != nullptr) *ok = isok;

        if(!isok)
            return meas;

        while(query.next()) {
            MeasData m;

            bool isok;
            m.time = query.value(0).toLongLong(&isok);
            if(!isok) continue;

            m.channel = query.value(1).toInt(&isok);
            if(!isok) continue;

            QList <double> mtie;
            QList <double> tdev;
            QList <double> rf;

            for(int i = 0; i < measInterval.size(); ++i) {
                mtie.append(query.value(static_cast<int>(Columns::Mtie) + i).isNull() ||
                            query.value(static_cast<int>(Columns::Mtie) + i) == "" ? NAN
                                                                                   : query.value(static_cast<int>(Columns::Mtie) + i).toDouble());
                tdev.append(query.value(static_cast<int>(Columns::Mtie) + i + measInterval.size()).isNull() ||
                            query.value(static_cast<int>(Columns::Mtie) + i + measInterval.size()) == "" ? NAN
                                                                                                         : query.value(static_cast<int>(Columns::Mtie) + i + measInterval.size()).toDouble());
                rf.append(query.value(static_cast<int>(Columns::Mtie) + i +  measInterval.size() * 2).isNull() ||
                          query.value(static_cast<int>(Columns::Mtie) + i +  measInterval.size() * 2) == "" ? NAN
                                                                                                            : query.value(static_cast<int>(Columns::Mtie) + i + measInterval.size() * 2).toDouble());
            }

            m.data.set(static_cast<int>(SSU::MeasDataType::Mtie), mtie);
            m.data.set(static_cast<int>(SSU::MeasDataType::Tdev), tdev);
            m.data.set(static_cast<int>(SSU::MeasDataType::Rf), rf);

            meas.append(m);
        }

        return meas;
    }

    QList<MeasData> Vch003MeasurementTable::select(Database * const db, int channel, qint64 startTime, qint64 endTime, int decimation, bool *ok) const
    {
        QStringList filters;

        if(channel != -1) {
            filters.append(columnName(Columns::Channel) +
                           " = " +
                           QString::number(channel));
        }

        if(startTime != 0) {
            filters.append(columnName(Columns::Time) +
                           " >= " +
                           QString::number(startTime));
        }
        if(endTime != 0) {
            filters.append(columnName(Columns::Time) +
                           " <= " +
                           QString::number(endTime));
        }

        if(decimation != 0) {
            filters.append( columnName(Columns::Time) +
                            " % " +
                            QString::number(decimation) +
                            " == 0");
        }

        return select(db, filters, ok);
    }

    QList<int> Vch003MeasurementTable::readReclist(Database * const db, qint64 startTime, qint64 endTime, int decimation, bool *ok) const
    {
        QList <int> reclist;
        bool isok;

        QStringList filters;
        if(startTime != 0) {
            filters.append(columnName(Columns::Time) +
                           " >= " +
                           QString::number(startTime));
        }
        if(endTime != 0) {
            filters.append(columnName(Columns::Time) +
                           " <= " +
                           QString::number(endTime));
        }

        if(decimation != 0) {
            filters.append( columnName(Columns::Time) +
                            " % " +
                            QString::number(decimation) +
                            " == 0");
        }

        QSqlQuery query = Table::select(db, filters, { "DISTINCT " + columnName(Columns::Channel) }, &isok);

        if(ok != nullptr) *ok = isok;

        if(!isok)
            return reclist;

        while(query.next()) {
            int value = query.value(0).toInt(&isok);
            if(isok)
                reclist.append(value);
        }

        return reclist;
    }

    QString Vch003MeasurementTable::columnName(Vch003MeasurementTable::Columns column)
    {
        switch (column) {
            case Columns::Time:         return "Time";
            case Columns::Channel:      return "Channel";
            case Columns::Mtie:         return "Mtie";
            case Columns::Tdev:         return "Tdev";
            case Columns::Rf:           return "Rf";
            default:
                break;
        }
        return "";
    }

    Vch003MeasurementLogTable::Vch003MeasurementLogTable(QObject *parent) : Vch003MeasurementTable(MEASUREMENT_LOG_TABLE, parent)
    {

    }

    Vch003CurrentMeasurementTable::Vch003CurrentMeasurementTable(QObject *parent) : Vch003MeasurementTable(CURRENT_MEASUREMENT_TABLE, parent)
    {

    }


}




