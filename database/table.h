#ifndef TABLE_H
#define TABLE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "database.h"

namespace ssu {
    class Table : public QObject
    {
        Q_OBJECT

    public:
        Table(const QString &name, QObject *parent = nullptr);
        virtual ~Table() = default;

        QString name() const;

        virtual void create(Database * const db, bool *ok = nullptr) const = 0;
        void drop(const Database * const db, bool *ok = nullptr);

        virtual QVariant lastValue(const Database * const db, const QStringList &filters, bool *ok = nullptr) const;
        QVariant lastValue(const Database * const db, bool *ok = nullptr) const;

        virtual int insert(Database * const db, const QVariantList &list, bool *ok = nullptr);

        // Удалить все и добавить заново
        int update(Database * const db, const QVariantList &list, bool *ok = nullptr);

        void updateRow(Database * const db, const QList<QPair<QString, QVariant> > &columns, const QStringList &filters, bool *ok = nullptr);

        QString selectQuery(const QStringList &filters = {}, const QStringList &values = {}) const;
        QSqlQuery select(const Database * const db, const QStringList &filters, bool *ok = nullptr) const;
        QSqlQuery select(const Database * const db, const QStringList &filters, const QStringList &values, bool *ok = nullptr) const;

        bool exists(const Database * const db) const;

    signals:
        void dataChanged();

    protected:
        QString where(const QStringList &filters) const;

    protected:
        const QString name_;
    };

}

#endif // TABLE_H
