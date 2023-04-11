#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QElapsedTimer>
#include <QTimer>

#include <QThread>
#include <QProcess>
#include <CTranslator>
#include <QIcon>

#ifdef DEBUG_REDIRECT_TO_FILE
#include "installmessagehandler.h"
#endif

#include "qcc.h"
#include "controller.h"
#include "csettings.h"


#include "programdatamanagementmodel.h"

#include "maskdata/concatenatemasklistmodel.h"


#include "vch003/log/eventlogmodel.h"
#include "vch003/log/activeeventmodel.h"
#include "vch003/log/hiddeneventmodel.h"
#include "vch003/log/useractivitylogmodel.h"

#include "vch003/log/measlogmodel.h"
#include "vch003/log/currentmeasurementsmodel.h"
#include "vch003/log/measurementsmodel.h"

#include "vch003/cards/gnss/satellitelistmodel.h"
#include "vch003/cards/timeprotocol/ethernetport.h"

using namespace ssu;











#include "vch003/vch003.h"
#include "backup/backuplistmodel.h"
#include "vch003/monitorring/request.h"
#include "vch003/monitorring/vch003requests.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <QSqlTableModel>

#include "database/database.h"
#include <QFile>
#include <QDir>
#include "device/configuration/lanconnection.h"
#include <QSqlError>
#include "maskdata/maskdata.h"



//class BaseData {
//public:
//    BaseData() {}
//    virtual ~BaseData() = default;
//private:
//    QList <double> values;
//};

//class Mtie : public BaseData {
//public:
//    Mtie() : BaseData() {}
//};

//class Tdev : public BaseData {
//public:
//    Tdev() : BaseData() {}
//};

//class Rf : public BaseData {
//public:
//    Rf() : BaseData() {}
//};

//class DataList {
//public:
//    DataList() {
//        d.append(new Mtie());
//        d.append(new Tdev());
//        d.append(new Rf());
//    }
//    ~DataList() {
//        qDeleteAll(d);
//    }
//private:
//    QList <BaseData*> d;
//};

//class DataHash {
//public:
//    DataHash() {
//        d.insert(0, new Mtie());
//        d.insert(1, new Tdev());
//        d.insert(2, new Rf());
//    }
//    ~DataHash() {
//        for(auto &data : d) {
//            delete data;
//        }
//    }
//private:
//    QHash <int, BaseData*> d;
//};



void registerTypes(QQmlApplicationEngine *engine) {
#ifdef ENABLE_APP_UPDATER
    //    AppUpdateWidget::registerTypes(engine);
    engine->rootContext()->setContextProperty("ENABLE_APP_UPDATER", true);
    engine->rootContext()->setContextProperty("updater", AppUpdater::getInstance());
#else
    engine->rootContext()->setContextProperty("ENABLE_APP_UPDATER", false);
#endif

    QCC::registerTypes(engine);

    Vch003::registerTypes();
    qmlRegisterSingletonType <Vch003>("SSU", 1, 0, "Vch003", &Vch003::getInstance);

    SSU::registerTypes();
    qmlRegisterSingletonType <SSU>("SSU", 1, 0, "SSU", &SSU::getInstance);

    qRegisterMetaType <ConnectionSettings::Status> ("ConnectionSettings::Status");




    // Gnss card Port
    qRegisterMetaType <EthernetPort::PpsSource> ("EthernetPort::PpsSource");
    qRegisterMetaType <EthernetPort::Speed> ("EthernetPort::Speed");
    qRegisterMetaType <Ptp::TxMode> ("Ptp::TxMode");
    qRegisterMetaType <Ptp::WayMode> ("Ptp::WayMode");
    qRegisterMetaType <Ptp::SyncMode> ("Ptp::SyncMode");
    qRegisterMetaType <Ptp::DelayMechanism> ("Ptp::DelayMechanism");
    qRegisterMetaType <Ptp::Protocol> ("Ptp::Protocol");
    qRegisterMetaType <Ptp::PtpMode> ("Ptp::PtpMode");


    qRegisterMetaType <PortStatistics> ("PortStatistics");
    qRegisterMetaType <DataContainer> ("DataContainer");
    qRegisterMetaType <MaskData> ("MaskData");
    qRegisterMetaType <SlotConfiguration> ("SlotConfiguration");

    qRegisterMetaType <QList <uint>> ("QList <uint>");
    qRegisterMetaType <QList<MeasData>> ("QList<MeasData>");

    qmlRegisterInterface<ReclistModel>("ReclistModel");
    qmlRegisterInterface<Device>("Device");
    qmlRegisterInterface<Card>("Card");
    qmlRegisterInterface<Channel>("Channel");
    qmlRegisterInterface<SsuPanelConfiguration>("SsuPanelConfiguration");
    qmlRegisterInterface<NetworkConfiguration>("NetworkConfiguration");

    qmlRegisterInterface<Request>("Request");
    qRegisterMetaType <QList<Request*>> ("QList<Request*>");


    qmlRegisterType <BackupListModel> ("Models", 1, 0, "BackupListModel");
    qmlRegisterType <ThresholdList> ("Models", 1, 0, "ThresholdList");
    qmlRegisterType <UserList> ("Models", 1, 0, "UserList");
    qmlRegisterType <EventDescriptionFilterProxyModel> ("Models", 1, 0, "EventDescriptionFilterProxyModel");
    qmlRegisterType <ProgramDataManagementModel> ("Models", 1, 0, "ProgramDataManagementModel");
    qmlRegisterType <SatelliteListModel> ("Models", 1, 0, "SatelliteListModel");

    qmlRegisterType <ChannelDescription> ("Models", 1, 0, "ChannelDescription");


    qmlRegisterType <EthernetPort> ("Models.Port", 1, 0, "EthernetPort");
    qmlRegisterType <Ptp> ("Models.Port", 1, 0, "Ptp");


    qmlRegisterType <ConcatenateMaskListModel> ("Models.Mask", 1, 0, "ConcatenateMaskListModel");

    qmlRegisterType <EventLogModel> ("Models.Log", 1, 0, "EventLogModel");
    qmlRegisterType <HiddenEventModel> ("Models.Log", 1, 0, "HiddenEventModel");
    qmlRegisterType <ActiveEventModel> ("Models.Log", 1, 0, "ActiveEventModel");
    qmlRegisterType <UserActivityLogModel> ("Models.Log", 1, 0, "UserActivityLogModel");


    qmlRegisterType <MeasurementsModel> ("Models.Meas", 1, 0, "MeasurementsModel");
    qmlRegisterType <MeasLogModel> ("Models.Meas", 1, 0, "MeasLogModel");
    qmlRegisterType <CurrentMeasurementsModel> ("Models.Meas", 1, 0, "CurrentMeasurementsModel");

    qmlRegisterType <SsuSlot> ("ManagerTypes", 1, 0, "SsuSlot");
    qmlRegisterType <ConnectionSettings> ("ConnectionSettings", 1, 0, "ConnectionSettings");
    qmlRegisterType <CardList> ("ManagerTypes", 1, 0, "CardList");
    qmlRegisterType <Led> ("ManagerTypes", 1, 0, "LedModel");
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);

    app.setApplicationName("SSUManager");
    app.setApplicationVersion(SOFTWARE_VERSION);
    app.setWindowIcon(QIcon(":/icons/icon.ico"));

    app.setOrganizationName("Vremya-Ch");
    app.setOrganizationDomain("vremya-ch.com");

#ifdef DEBUG_REDIRECT_TO_FILE
    QDir d;
    d.mkpath(DEBUG_LOG_DIR_PATH);
    qInstallMessageHandler(messageOutputHandler);
#endif

    qInfo() << app.applicationName() << app.applicationVersion();

    qDebug() << "QT_DEBUG_OUTPUT";
    qInfo() << "QT_INFO_OUTPUT";
    qWarning() << "QT_WARNING_OUTPUT";


    QQmlApplicationEngine engine;

    // перевод
    QObject::connect(CTranslator::getInstance(), &CTranslator::currentLanguageChanged, CSettings::getInstance(), &CSettings::setCurrentLanguage);
    CTranslator::getInstance()->init(QLocale::English, &app, &engine);
    CTranslator::getInstance()->appendLanguage(QLocale::Russian, ":/ts/ssumanager_ru.qm");

    auto strLanguage = CSettings::get(GENERAL_SETTINGS_GROUP, "Language", QLocale::Russian).toString();
    if(!strLanguage.isEmpty()) {
        bool isok;
        auto language = static_cast <QLocale::Language>(strLanguage.toInt(&isok));
        if(isok) CTranslator::getInstance()->setCurrentLanguage(language);
    }

    Controller *controller = new Controller;


    engine.addImportPath("qrc:");
    engine.addImportPath("qrc:/device");

    registerTypes(&engine);

    engine.rootContext()->setContextProperty("programSettings", CSettings::getInstance());
    engine.rootContext()->setContextProperty("translator", CTranslator::getInstance());
    engine.rootContext()->setContextProperty("controller", controller);

    const QUrl url(QStringLiteral("qrc:/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    int result = app.exec();
    delete controller;

    if(result == APP_RESTART_CODE) {
        QElapsedTimer time; time.start();
        while(controller->device() != nullptr && time.elapsed() < 5000) {    // Пока так..
            QEventLoop loop; QTimer::singleShot(100, &loop, SLOT(quit())); loop.exec();
        }

        QString program = qApp->arguments()[0];
        QStringList arguments = qApp->arguments().mid(1); // remove the 1st argument - the program name
        QProcess::startDetached(program, arguments);
        result = 0;
    }

    qInfo() << "THE END" << result;


    return result;
}


