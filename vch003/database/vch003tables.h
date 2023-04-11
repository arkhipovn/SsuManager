#ifndef VCH003TABLES_H
#define VCH003TABLES_H

#include "vch003/data/vch003event.h"
#include "vch003/data/vch003useractivity.h"
#include "data/measdata.h"
#include "database/table.h"
#include "ssu.h"

namespace ssu {
    //------- Общий класс таблиц с событиями -------

    class Vch003EventTable : public Table
    {
    public:
        Vch003EventTable(const QString &name, QObject *parent = nullptr);
        virtual ~Vch003EventTable() = default;

        void create(Database * const db, bool *ok = nullptr) const override;
        QVariant lastValue(const Database * const db, const QStringList &filters, bool *ok = nullptr) const override;

        int insert(Database * const db, const QVariantList &list, bool *ok = nullptr) override;
        int insert(Database * const db, const QList <Vch003Event> &events, bool *ok = nullptr);

        QList <Vch003Event> select(Database * const db, const QStringList &filters, bool *ok = nullptr) const;

        // Колонки таблицы
        enum class Columns {
            Hash = 0,
            OccurredTime,
            ClearedTime,
            Slot,
            CardType,
            EventId,
            Channel,
            Parameters,
            Comment,
            NotificationStatus      // Если сообщение подтверждено, то будет записано true, если нет, то false
        };

        static QString columnName(Columns column);
    };

    //------- Класс таблицы журнала событий -------

    class Vch003EventLogTable : public Vch003EventTable
    {
    public:
        Vch003EventLogTable(QObject *parent = nullptr);
    };



    //------- Класс таблицы активых событий -------

    class Vch003ActiveEventsTable : public Vch003EventTable
    {
    public:
        Vch003ActiveEventsTable(QObject *parent = nullptr);
    };



    //------- Класс таблицы скрытых событий -------

    class Vch003HiddenEventsTable : public Vch003EventTable
    {
    public:
        Vch003HiddenEventsTable(QObject *parent = nullptr);
    };


    //------- Класс таблицы действий пользователя -------

    class Vch003UserActivityLogTable : public Table
    {
    public:
        Vch003UserActivityLogTable(QObject *parent = nullptr);

        void create(Database * const db, bool *ok = nullptr) const override;
        QVariant lastValue(const Database * const db, const QStringList &filters, bool *ok = nullptr) const;

        int insert(Database * const db, const QVariantList &list, bool *ok = nullptr) override;
        int insert(Database * const db, const QList <Vch003UserActivity> &activities, bool *ok = nullptr);

        QList <Vch003UserActivity> select(Database * const db, const QStringList &filters, bool *ok = nullptr) const;

        // Колонки таблицы
        enum class Columns {
            Hash = 0,
            OccurredTime,
            Address,
            Username,
            Action,
            NotificationStatus      // Если сообщение подтверждено, то будет записано true, если нет, то false
        };

        static QString columnName(Columns column);
    };


    //------- Класс таблицы с измерениями -------

    //@N переделать бы вот эту таблицу..
    class Vch003MeasurementTable : public Table
    {
    public:
        Vch003MeasurementTable(const QString &name, QObject *parent = nullptr);
        virtual ~Vch003MeasurementTable() = default;

        void create(Database * const db, bool *ok = nullptr) const override;
        QVariant lastValue(const Database * const db, const QStringList &filters, bool *ok = nullptr) const override;
        QVariant lastValue(const Database * const db, int channel, bool *ok = nullptr) const;

        int insert(Database * const db, const QVariantList &list, bool *ok = nullptr) override;
        int insert(Database * const db, const QList <MeasData> &meas, bool *ok = nullptr);

        QList <MeasData> select(Database * const db, const QStringList &filters = {}, bool *ok = nullptr) const;

        /*
        decimation прореживание в сек
        */
        QList <MeasData> select(Database * const db, int channel, qint64 startTime = 0, qint64 endTime = 0, int decimation = 0, bool *ok = nullptr) const;
        QList <int> readReclist(Database * const db, qint64 startTime = 0, qint64 endTime = 0, int decimation = 0, bool *ok = nullptr) const;

        // Колонки таблицы
        enum class Columns {
            Time = 0,
            Channel,
            Mtie,
            Tdev,
            Rf
        };

        static QString columnName(Columns column);
    };


    //------- Класс таблицы журнала измерений -------

    class Vch003MeasurementLogTable : public Vch003MeasurementTable
    {
    public:
        Vch003MeasurementLogTable(QObject *parent = nullptr);
    };

    //------- Класс таблицы текущих измерений -------

    class Vch003CurrentMeasurementTable : public Vch003MeasurementTable
    {
    public:
        Vch003CurrentMeasurementTable(QObject *parent = nullptr);
    };



}




#endif // VCH003TABLES_H
