#include "databaseexecutor.h"
#include <QDebug>
#include "ssu.h"

namespace ssu {
    DatabaseExecutor::DatabaseExecutor(Database *database) :
        db_(database)
    {

    }

    DatabaseExecutor::~DatabaseExecutor()
    {
        qDeleteAll(tables_.values());
        tables_.clear();
    }

    void DatabaseExecutor::insertTable(int tableId, Table *table)
    {
        if(tables_.contains(tableId)) {
            delete table;       // уже есть..
        }
        else
            tables_.insert(tableId, table);
    }

    Table *DatabaseExecutor::table(int tableId) const
    {
        auto it = tables_.find(tableId);
        return it == tables_.end() ? nullptr
                                   : it.value();
    }

    void DatabaseExecutor::create(int tableId, bool *ok) const
    {
        auto it = tables_.constFind(tableId);
        if(it == tables_.constEnd()) {
            if(ok != nullptr) *ok = false;
        }
        else {
            it.value()->create(db_, ok);
        }
    }

    void DatabaseExecutor::drop(int tableId, bool *ok)
    {
        auto it = tables_.constFind(tableId);
        if(it == tables_.constEnd()) {
            if(ok != nullptr) *ok = false;
        }
        else {
            it.value()->drop(db_, ok);
            vacuum();
        }
    }

    bool DatabaseExecutor::vacuum()
    {
        if(db_ == nullptr)
            return false;
        return db_->vacuum();
    }

    QVariant DatabaseExecutor::lastValue(int tableId, const QStringList &filters, bool *ok) const
    {
        QVariant var;
        auto it = tables_.constFind(tableId);
        if(it == tables_.constEnd()) {
            if(ok != nullptr) *ok = false;
        }
        else {
            var = it.value()->lastValue(db_, filters, ok);
        }
        return var;
    }

    QVariant DatabaseExecutor::lastValue(int tableId, bool *ok) const
    {
        return lastValue(tableId, {}, ok);
    }

    int DatabaseExecutor::insertRows(int tableId, const QVariantList &list, bool *ok)
    {
        int count = 0;
        auto it = tables_.constFind(tableId);

        if(it == tables_.constEnd()) {
            if(ok != nullptr) *ok = false;
        }
        else {
            count = it.value()->insert(db_, list, ok);
        }

        return count;
    }

    int DatabaseExecutor::updateRows(int tableId, const QVariantList &list, bool *ok)
    {
        int count = 0;
        auto it = tables_.constFind(tableId);

        if(it == tables_.constEnd()) {
            if(ok != nullptr) *ok = false;
        }
        else {
            count = it.value()->update(db_, list, ok);
        }
        return count;
    }
}
