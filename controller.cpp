#include "controller.h"
#include "vch003/vch003.h"
#include "vch003/vch003device.h"
#include "device/configuration/lanconnection.h"
#include "ssu.h"

#include <QtMath>
#include <QSettings>
#include <QFileDialog>

#include "vch003/cards/inputcard.h"

namespace ssu {

    Controller::Controller(QObject *parent) : QObject(parent)
  #ifdef ENABLE_APP_UPDATER
      ,
        updateWidget_(nullptr)
  #endif
    {
#ifdef ENABLE_APP_UPDATER
        int interval = 3600000;// CSettings::get("UPDATER", "Interval", 3600000).toInt();
        bool isBackupCreating = CSettings::get("UPDATER", "IsBackupCreating", true).toBool();
        bool isRunning = CSettings::get("UPDATER", "AutoVersionCheck", true).toBool();

        QObject::connect(AppUpdater::getInstance(), &AppUpdater::makeBackupChanged, this, &Controller::setProgramUpdateSettings);
        QObject::connect(AppUpdater::getInstance(), &AppUpdater::runningChanged, this, &Controller::setProgramUpdateSettings);

        if(isRunning) AppUpdater::getInstance()->start(interval, isBackupCreating);
        else {
            AppUpdater::getInstance()->setInterval(interval);
            AppUpdater::getInstance()->creatingBackup(isBackupCreating);
        }

        QObject::connect(AppUpdater::getInstance(), &AppUpdater::newVersionFound, this, &Controller::openUpdateWidget);
        QObject::connect(AppUpdater::getInstance(), &AppUpdater::newVersionApplicationReadyForInstallation, this, &Controller::openUpdateWidget);
#endif


        measMaskList_ = new MeasMaskList(this);
        measMaskList_->setPath(SSU::maskDataDirPath());

        //____________ Первый запуск ______________
        QDir maskDataDir(measMaskList_->path());
        if(!QFile::exists(SSU::programSettingsFile()) && !maskDataDir.exists())
            measMaskList_->createDefaultMasks({ 1, 10, 100, 1000, 10000 }); //@N intervals..
        //_________________________________________



        //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..
        auto lan = new LanConnection;

        if(SSU::kImitationWork) {
            lan->setConnectionParameters("127.0.0.1", 5005);
            lan->setAuthorizationParameters("SEC", "SEC");
        }
        else {
            QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
            s.setIniCodec("UTF-8");
            s.beginGroup(CONNECTION_SETTINGS_GROUP);
            lan->setConnectionParameters(s.value("Hostname", "").toString(),
                                         s.value("Port", 5005).toUInt());
            lan->setAuthorizationParameters(s.value("Username", "").toString(),
                                            s.value("Password", "").toString());
            lan->setStoreUserInformation(s.value("StoreUserInformation", "").toBool());
            lan->setAutoConnect(s.value("AutoConnect", "").toBool());
            s.endGroup();
        }

        device_ = new Vch003Device(lan, this);

        if(SSU::kImitationWork) {
            device_->connectionSettings()->setLevel(SSU::UserLevel::Security);
            //            device_->connectionSettings()->setLevel(SSU::UserLevel::User);

            // Inventory
            device_->panel()->inventory()->update({
                                                      { SSU::Input, 0, "sn0", "sw0", "hw0" },
                                                      { SSU::Input, 1, "sn1", "sw1", "hw1" },
                                                      { SSU::Input, 2, "sn2", "sw2", "hw2" },

                                                      { SSU::Gnss, 4, "sn1", "sw1", "hw1" },

                                                      { SSU::Generator, 6, "sn5", "sw5", "hw5" },
                                                      { SSU::Generator, 8, "sn5", "sw5", "hw5" },

                                                      { SSU::Output, 10, "sn6", "sw6", "hw6" },

                                                      { SSU::Distribution, 12, "sn7", "sw7", "hw7" },
                                                      { SSU::Distribution, 13, "sn6", "sw6", "hw6" },
                                                      { SSU::Distribution, 15, "sn6", "sw6", "hw6" },
                                                      { SSU::Distribution, 16, "sn7", "sw7", "hw7" },
                                                      { SSU::Distribution, 17, "sn6", "sw6", "hw6" },

                                                      { SSU::Monitoring, 18, "sn8", "sw8", "hw8" }
                                                  });


            // UserList
            device_->userList()->set({
                                         { "USER",      SSU::UserLevel::User },
                                         { "USER2",     SSU::UserLevel::User, QDateTime::currentDateTimeUtc().addDays(2).toSecsSinceEpoch() },
                                         { "NIKITA",    SSU::UserLevel::Admin, 0, true },
                                         { "SEC",       SSU::UserLevel::Security },
                                         { "Test",      SSU::UserLevel::Admin, 0, true },
                                     });
        }

        //        if(false) {
        //            // SSU dev id 1, 2, 5, 6, 7
        //            const QString connectionName = "ms_db";
        //            const int devId = 1;

        //            /* ManagementSystem db */
        //            {

        //                QSqlDatabase ms_db = QSqlDatabase::addDatabase("QPSQL", connectionName);
        //                ms_db.setDatabaseName("TestPostgre");
        //                ms_db.setUserName("postgres");
        //                ms_db.setHostName("192.168.124.62");
        //                ms_db.setPassword("");

        //                if(ms_db.open()) {
        //                    qInfo() << "DB OK!";
        //                    QSqlQuery query(ms_db);


        //                    // Write homeapp_eventstate

        //                    {
        //                        bool isok = ms_db.transaction();
        //                        if(isok) {
        //                            query.prepare( { "INSERT INTO 'homeapp_eventstate'"
        //                                             " VALUES(?,?,?)"});

        //                            auto it = device_->eventDescriptionList()->constBegin();
        //                            while(it != device_->eventDescriptionList()->constEnd()) {
        //                                query.addBindValue(it->description());
        //                                query.addBindValue(static_cast<int>(it->status()));
        //                                query.addBindValue(devId);

        //                                if(!query.exec()) {
        //                                    ms_db.rollback();
        //                                    isok = false;
        //                                    qInfo() << "rollback";
        //                                    break;
        //                                }
        //                                ++it;
        //                            }
        //                        }

        //                        if(isok) {
        //                            qInfo() << "commit";
        //                            ms_db.commit();
        //                        }
        //                    }

        //                    // homeapp_currentevent
        //                    {
        //                        bool isok = ms_db.transaction();
        //                        if(isok) {
        //                            query.prepare( { "INSERT INTO 'homeapp_currentevent'"
        //                                             " VALUES(?,?,?,?,?,?,?)"});
        //                            auto vch003 = static_cast<Vch003Device*>(device_);

        //                            auto it = vch003->activeEvents()->constBegin();
        //                            while(it != vch003->activeEvents()->constEnd()) {
        //                                query.addBindValue(it->occurredTime);
        //                                query.addBindValue(it->clearedTime);
        //                                query.addBindValue(0);
        //                                query.addBindValue("");
        //                                query.addBindValue(false);
        //                                query.addBindValue(devId);
        //                                //                            query.addBindValue(); //eventId

        //                                if(!query.exec()) {
        //                                    ms_db.rollback();
        //                                    isok = false;
        //                                    qInfo() << "rollback";
        //                                    break;
        //                                }
        //                                ++it;
        //                            }
        //                        }

        //                        if(isok) {
        //                            qInfo() << "commit";
        //                            ms_db.commit();
        //                        }
        //                    }

        //                    ms_db.close();
        //                }
        //            }

        //            {
        //                QSqlDatabase::removeDatabase(connectionName);
        //            }
        //        }


    }

    Controller::~Controller()
    {
        qInfo() << "~Controller() start";

        device_->disconnectFromDevice();
        delete device_;
        device_ = nullptr;

        qInfo() << "~Controller() end";
    }

    void Controller::closeEvent()
    {
        device_->disconnectFromDevice();

#ifdef ENABLE_APP_UPDATER
        //@N Потом переделать вот это. Добавить окно обновлений на qml..
        if(updateWidget_ != nullptr) delete updateWidget_;

        if(AppUpdater::getInstance()->readyToInstall()) {
            QMessageBox m;
            m.setWindowTitle(tr("Installing the update"));
            m.setText(tr("Install updates now?"));

            m.addButton(tr("Yes"), QMessageBox::YesRole);
            m.addButton(tr("No"), QMessageBox::NoRole);

            m.setIcon(QMessageBox::Question);
            auto reply = m.exec();
            if(reply == 0) AppUpdater::getInstance()->installUpdate(CTranslator::getInstance()->getCurrentLanguage());
        }
#endif

        qApp->exit();
    }

    MeasMaskList *Controller::measMaskList() const
    {
        return measMaskList_;
    }

    Device *Controller::device() const
    {
        return device_;
    }

    void Controller::restartApplication()
    {
        device()->disconnectFromDevice();
        qApp->exit(APP_RESTART_CODE);
    }

    void Controller::connectToDevice(Device *device, const QString &hostname, quint16 port)
    {
        //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..
        auto lan = qobject_cast<LanConnection*>(device->connectionSettings());
        lan->setConnectionParameters(hostname, port);
        saveConnectionSettings();
        device->connectToDevice();
    }

    void Controller::authorization(Device *device, const QString &username, const QString &password, bool save)
    {
        device->connectionSettings()->setAuthorizationParameters(username, password);
        device->connectionSettings()->setStoreUserInformation(save);
        saveAuthorizationParameters();
        device->authorization();
    }

    void Controller::disconnectFromDevice(Device *device)
    {
        device->disconnectFromDevice();
    }

    void Controller::setAutoConnect(Device *device, bool value)
    {
        if(device->connectionSettings()->autoConnect() != value) {
            device->connectionSettings()->setAutoConnect(value);
            CSettings::set(CONNECTION_SETTINGS_GROUP, "AutoConnect", value);
        }
    }

    QString Controller::getOpenFileName(const QString &filters) const
    {
        return QFileDialog::getOpenFileName(0, tr("Select file"), QDir::currentPath(),
                                            filters);
    }

#ifdef ENABLE_APP_UPDATER
    void Controller::openUpdateWidget()
    {
        if(updateWidget_ != nullptr) return;
        updateWidget_ = new UpdaterWidget(QLocale::Russian);
        QObject::connect(updateWidget_, &UpdaterWidget::widgetClosed, updateWidget_, &UpdaterWidget::deleteLater);
        QObject::connect(updateWidget_, &UpdaterWidget::closeTheProgram, qApp, &QApplication::quit);
        QObject::connect(updateWidget_, &UpdaterWidget::destroyed, [&]() { updateWidget_ = nullptr; });
        updateWidget_->show();
    }

    void Controller::setProgramUpdateSettings()
    {
        CSettings::set(UPDATER_GROUP, "AutoVersionCheck", AppUpdater::getInstance()->isRanning());
        CSettings::set(UPDATER_GROUP, "IsBackupCreating", AppUpdater::getInstance()->isBackupCreating());
    }
#endif

    void Controller::saveConnectionSettings()
    {
        if(device_->connectionSettings() == nullptr)
            return;

        //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..
        auto lan = qobject_cast<LanConnection*>(device_->connectionSettings());

        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.beginGroup(CONNECTION_SETTINGS_GROUP);

        s.setValue("Hostname", lan->hostname());
        s.setValue("Port", lan->port());

        s.setValue("StoreUserInformation", lan->storeUserInformation());

        if(lan->storeUserInformation()) {
            s.setValue("Username", lan->username());
            s.setValue("Password", lan->password());
        }
        else {
            s.setValue("Username", "");
            s.setValue("Password", "");
        }
        s.endGroup();
    }

    void Controller::saveAuthorizationParameters()
    {
        if(device_->connectionSettings() == nullptr)
            return;

        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.beginGroup(CONNECTION_SETTINGS_GROUP);

        s.setValue("StoreUserInformation", device_->connectionSettings()->storeUserInformation());

        if(device_->connectionSettings()->storeUserInformation()) {
            s.setValue("Username", device_->connectionSettings()->username());
            s.setValue("Password", device_->connectionSettings()->password());
        }
        else {
            s.setValue("Username", "");
            s.setValue("Password", "");
        }
        s.endGroup();
    }

}
