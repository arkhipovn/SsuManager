#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <QVariant>

namespace ssu {    
    class Database
    {
    public:
        Q_DISABLE_COPY(Database)

        Database();
        virtual ~Database();

        bool open(const QString &name, const QString &ver, const QString &id);
        void close();

        bool isCreated() const;
        bool isValid() const;
        bool isOpen() const;

        QSqlDatabase database() const;
        QString databaseName() const;

        operator QSqlDatabase() const;

        bool transaction();
        bool commit();
        bool rollback();
        bool vacuum();

        bool create();

        // Проверить таблицу с версиями. Это нужно для того, чтобы понять вдруг изменился формат таблиц
        QString databaseVersion() const;
        QString databaseId() const;
        QHash <QString, QVariant> databaseInfo() const;

    protected:
        QString version_;   // Версия базы. Если таблицы нужно поменять, то поменять версию базы. Тогда старая база удалится..
        QString id_;        // Идентификатор устройства

        QString databaseName_;
        QSqlDatabase db_;
        bool setup();

        void updateDatabaseInfo();

        enum class CreateResult {
            Error,
            Created,
            Exists
        };

        CreateResult createDatabaseFile();

        void dropTables();
    };
}

#endif // DATABASE_H





