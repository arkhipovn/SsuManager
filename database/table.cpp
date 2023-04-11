#include "table.h"
#include <QDebug>

namespace ssu {

    Table::Table(const QString &name, QObject *parent) : QObject(parent),
        name_(name)
    {

    }

    QString Table::name() const
    {
        return name_;
    }

    void Table::drop(const Database * const db, bool *ok)
    {
        if(ok != nullptr) *ok = false;
        if(db == nullptr)
            return;
        if(name_.isEmpty())
            return;
        if(!db->isCreated() || !db->isOpen())
            return;
        QSqlQuery query(*db);
        if(query.exec("DROP TABLE IF EXISTS '" + name_ + "'")) {
            if(ok != nullptr) *ok = true;
            emit dataChanged();
        }
    }

    QVariant Table::lastValue(const Database * const db, const QStringList &filters, bool *ok) const
    {
        Q_UNUSED(filters)
        Q_UNUSED(db)
        if(ok != nullptr) *ok = true;
        return QVariant();
    }

    QVariant Table::lastValue(const Database * const db, bool *ok) const
    {
        return lastValue(db, {}, ok);
    }

    int Table::insert(Database * const db, const QVariantList &list, bool *ok)
    {
        Q_UNUSED(db)
        Q_UNUSED(list)
        if(ok != nullptr) *ok = true;
        return 0;
    }

    int Table::update(Database * const db, const QVariantList &list, bool *ok)
    {
        int count = 0;
        if(ok != nullptr) *ok = false;
        if(db == nullptr)
            return count;
        if(name_.isEmpty())
            return count;
        if(!db->isCreated() || !db->isOpen())
            return count;

        bool changed = false;

        QSqlQuery query(*db);
        if(query.exec("DROP TABLE IF EXISTS '" + name_ + "'")) {
            if(ok != nullptr) *ok = true;
            changed = true;
            count = insert(db, list, ok);
        }

        if(changed) {
            emit dataChanged();
        }

        return count;
    }

    void Table::updateRow(Database * const db, const QList<QPair<QString, QVariant> > &columns, const QStringList &filters, bool *ok)
    {
        if(ok != nullptr) *ok = false;
        if(!db->isCreated() || !db->isOpen())
            return;
        if(columns.isEmpty())
            return;

        QString q = "UPDATE '" + name_ + "' SET ";
        for(int i = 0; i < columns.size(); ++i) {
            if(i > 0) q.append(", ");
            q.append(columns.at(i).first + " = '" + columns.at(i).second.toString() + "'");
        }

        q.append(where(filters));
        QSqlQuery query(q, *db);
        bool isok = query.exec();
        //@N наверное не буду тут выдавать сигнал dataChanged
        if(ok != nullptr) *ok = isok;
    }

    QString Table::selectQuery(const QStringList &filters, const QStringList &values) const
    {
        if(name_.isEmpty())
            return "";
        QString v = values.isEmpty() ? "*"
                                     : values.join(", ");
        return "SELECT " + v + " FROM '" + name_ + "'" + where(filters);
    }

    QSqlQuery Table::select(const Database * const db, const QStringList &filters, bool *ok) const
    {
        return select(db, filters, {}, ok);
    }

    QSqlQuery Table::select(const Database * const db, const QStringList &filters, const QStringList &values, bool *ok) const
    {
        if(ok != nullptr) *ok = false;
        QSqlQuery query(*db);
        if(!db->isCreated() || !db->isOpen())
            return query;

        QString q = selectQuery(filters, values);
        if(!q.isEmpty()) {
            bool isok = query.exec(q);
            if(ok != nullptr) *ok = isok;
        }

        return query;
    }

    bool Table::exists(const Database * const db) const
    {
        if(!db->isCreated() || !db->isOpen())
            return false;
        QSqlQuery query({ "SELECT name "
                          "FROM sqlite_master "
                          "WHERE type = 'table' "
                          "AND name = '" + name_ + "'"},
                        *db);
        if(query.exec()) {
            return query.next();
        }
        return false;
    }

    QString Table::where(const QStringList &filters) const
    {
        if(filters.isEmpty())
            return "";
        const QString filter = filters.join(" AND ");
        return filter.isEmpty() ? ""
                                : " WHERE " + filter;
    }
}
