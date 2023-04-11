#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "ssu.h"

namespace ssu {

    Database::Database()
    {

    }

    Database::~Database()
    {
        close();
    }

    bool Database::open(const QString &name, const QString &ver, const QString &id)
    {
        if(!databaseName_.isEmpty())
            close();
        version_ = ver;
        id_ = id;
        databaseName_ = name;
        return setup();
    }

    void Database::close()
    {
        if(db_.isOpen()) {
            db_.close();
        }
        db_ = QSqlDatabase();

        if(!databaseName_.isEmpty()) {
            QSqlDatabase::removeDatabase(databaseName_);
        }

        databaseName_.clear();
        version_.clear();
        id_.clear();
    }

    bool Database::isCreated() const
    {
        if(databaseName_.isEmpty())
            return false;
        return QFileInfo::exists(databaseName_);
    }

    bool Database::isValid() const
    {
        return !databaseName_.isEmpty() && db_.isValid();
    }

    bool Database::isOpen() const
    {
        return db_.isOpen();
    }

    QSqlDatabase Database::database() const
    {
        return db_;
    }

    QString Database::databaseName() const
    {
        return databaseName_;
    }

    Database::operator QSqlDatabase() const
    {
        return db_;
    }

    bool Database::transaction()
    {
        return db_.transaction();
    }

    bool Database::commit()
    {
        return db_.commit();
    }

    bool Database::rollback()
    {
        return db_.rollback();
    }

    bool Database::vacuum()
    {
        if(!isCreated() || !isOpen())
            return false;
        QSqlQuery query(db_);
        return query.exec("vacuum");
    }

    bool Database::create()
    {
        auto res = createDatabaseFile();
        if(res == CreateResult::Created) {
            // Нужно создать таблицу с информацией о БД
            updateDatabaseInfo();
        }
        return res != CreateResult::Error;
    }

    bool Database::setup()
    {
        if(databaseName_.isEmpty())
            return false;

        const QString driver = "QSQLITE";

        if(!QSqlDatabase::isDriverAvailable(driver)) {
            qWarning() << "Driver " << driver << " is not available.";
            return false;
        }

        // Создадим файл БД если его нет
        auto res = createDatabaseFile();

        if(res == CreateResult::Error) {
            qWarning() << "Path not created";
            return false;
        }

        db_ = QSqlDatabase::addDatabase(driver, databaseName_);
        db_.setDatabaseName(databaseName_);

        if(!db_.open()) {
            qWarning() << "Error in opening DB" << db_.databaseName()
                       << "reason:" << db_.lastError().text();
            return false;
        }

        switch (res) {
            case CreateResult::Created:
                // Нужно обновить информацию о БД
                updateDatabaseInfo();
                break;
            case CreateResult::Exists: {
                // Нужно сверить совпадают ли версии..
                auto info = databaseInfo();

                QVariant ver = info.value("version");
                QVariant id = info.value("id");

                if(ver.isNull() || ver.toString() != version_ ||
                        id.isNull() || id.toString() != id_) {
                    dropTables();
                    // Нужно обновить информацию о БД
                    updateDatabaseInfo();
                    vacuum();
                }
            }
                break;
            default:
                break;
        }

        return true;
    }

    QHash <QString, QVariant> Database::databaseInfo() const
    {
        QHash <QString, QVariant> values;
        if(!isCreated() || !isOpen())
            return values;
        QSqlQuery query("SELECT * FROM '" + QString(DATABASE_INFO_TABLE) + "'", db_);
        if(query.exec()) {
            while(query.next()) {
                values.insert(query.value(0).toString(), query.value(1));
            }
        }
        return values;
    }

    QString Database::databaseVersion() const
    {
        QString version;
        if(!isCreated() || !isOpen())
            return version;
        QSqlQuery query("SELECT Value FROM '" + QString(DATABASE_INFO_TABLE) + "' WHERE Name = 'version'", db_);

        if(query.exec()) {
            if(query.next()) {
                version = query.value(0).toString();
            }
        }
        return version;
    }

    QString Database::databaseId() const
    {
        QString id;
        if(!isCreated() || !isOpen())
            return id;
        QSqlQuery query("SELECT Value FROM '" + QString(DATABASE_INFO_TABLE) + "' WHERE Name = 'id'", db_);
        if(query.exec()) {
            if(query.next()) {
                id = query.value(0).toString();
            }
        }
        return id;
    }

    void Database::updateDatabaseInfo()
    {
        if(!isCreated() || !isOpen())
            return;
        QSqlQuery query(db_);
        if(!query.exec("CREATE TABLE IF NOT EXISTS '" + QString(DATABASE_INFO_TABLE) + "' (Name TEXT, Value TEXT, PRIMARY KEY(Name))")) {
            return;
        }

        query.prepare("INSERT OR IGNORE INTO '" + QString(DATABASE_INFO_TABLE) + "'"
                                                                                 " VALUES(?,?)");

        query.addBindValue("version");
        query.addBindValue(QString(version_));
        query.exec();

        query.addBindValue("id");
        query.addBindValue(QString(id_));
        query.exec();
    }

    Database::CreateResult Database::createDatabaseFile()
    {
        if(databaseName_.isEmpty())
            return CreateResult::Error;

        QFileInfo info(databaseName_);
        // проверим есть ли файл базы данных..
        if(info.exists())
            return CreateResult::Exists;

        if(!info.absoluteDir().exists()) {
            if(!QDir().mkpath(info.absolutePath())) {   //  если нет, то создадим путь..
                return CreateResult::Error;
            }
        }

        // создаем файл
        QFile f(info.absoluteFilePath());
        if(f.open(QIODevice::WriteOnly)) {
            f.close();
        }
        else {
            return CreateResult::Error;
        }

        return CreateResult::Created;
    }

    void Database::dropTables()
    {
        if(!isCreated() || !isOpen()) return;
        QSqlQuery query(db_);
        for(auto table : db_.tables()) {
            query.exec("DROP TABLE IF EXISTS '" + table + "'");
        }
    }

}
