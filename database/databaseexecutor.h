#ifndef DATABASEEXECUTOR_H
#define DATABASEEXECUTOR_H

#include "table.h"

namespace ssu {
    class DatabaseExecutor
    {
    public:
        explicit DatabaseExecutor(Database *database);
        ~DatabaseExecutor();

        void insertTable(int tableId, Table *table);
        Table *table(int tableId) const;

        void create(int tableId, bool *ok = nullptr) const;
        void drop(int tableId, bool *ok = nullptr);
        QVariant lastValue(int tableId, const QStringList &filters, bool *ok = nullptr) const;
        QVariant lastValue(int tableId, bool *ok = nullptr) const;

        int insertRows(int tableId, const QVariantList &list, bool *ok = nullptr);

        // Обновить строки таблицы
        int updateRows(int tableId, const QVariantList &list, bool *ok = nullptr);

    private:
        Database *db_;
        QHash <int, ssu::Table*> tables_;

        bool vacuum();
    };

}

#endif // DATABASEEXECUTOR_H
