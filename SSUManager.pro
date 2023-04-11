QT += quick sql network printsupport

CONFIG += c++11

#DEFINES += DEVICE_TESTING_FORMS        # тесты выходного сигнала
DEFINES += ENABLE_APP_UPDATER

# Отключение дебага
DEFINES += QT_NO_DEBUG_OUTPUT
#DEFINES += QT_NO_INFO_OUTPUT
#DEFINES += QT_NO_WARNING_OUTPUT

# Перенаправлять дебаг в файл
#DEFINES += DEBUG_REDIRECT_TO_FILE


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += \
        backup/backup.cpp \
        backup/backuplistmodel.cpp \
        csettings.cpp \
        data/datacontainer.cpp \
        database/databaseexecutor.cpp \
        database/database.cpp \
        database/table.cpp \
        device/configuration/channeldescription.cpp \
        device/configuration/ssuslot.cpp \
        deviceoptionsmapper.cpp \
        log/abstractdevicelogmodel.cpp \
        log/eventdescription.cpp \
        log/eventdescriptionfilterproxymodel.cpp \
        log/eventdescriptionlistmodel.cpp \
        maskdata/abstractmasklistmodel.cpp \
        maskdata/concatenatemasklistmodel.cpp \
        maskdata/maskdata.cpp \
        maskdata/maskrecommendation.cpp \
        device/configuration/connectionsettings.cpp \
        device/configuration/lanconnection.cpp \
        device/panel/card.cpp \
        device/panel/cardfactory.cpp \
        device/panel/cardlist.cpp \
        device/panel/panel.cpp \
        device/configuration/deviceoptions.cpp \
        device/configuration/slotconfiguration.cpp \
        device/configuration/ssupanelconfiguration.cpp \
        device/configuration/userrights.cpp \
        device/device.cpp \
        device/led/led.cpp \
        device/led/ledlist.cpp \
        controller.cpp \
        device/user/user.cpp \
        device/user/userlist.cpp \
        maskdata/measmasklist.cpp \
        maskdata/thresholdlist.cpp \
        models/filelistmodel.cpp \
        programdatamanagementmodel.cpp \
        vch003/cards/gnss/gnssmodule.cpp \
        vch003/cards/timeprotocol/ethernetport.cpp \
        vch003/cards/timeprotocol/networkconfiguration.cpp \
        vch003/cards/timeprotocol/ntp.cpp \
        vch003/cards/timeprotocol/ptp.cpp \
        vch003/cards/vch003card.cpp \
        vch003/channels/inchannelgnsscard.cpp \
        vch003/data/vch003event.cpp \
        vch003/cards/distributioncard.cpp \
        vch003/cards/generatorcard.cpp \
        vch003/cards/gnss/satellitelistmodel.cpp \
        vch003/cards/gnsscard.cpp \
        vch003/cards/inputcard.cpp \
        vch003/cards/monitoringcard.cpp \
        vch003/cards/outputcard.cpp \
        vch003/cards/powercard.cpp \
        vch003/channels/inchannelgeneratorcard.cpp \
        vch003/channels/inchannelinputcard.cpp \
        vch003/channels/inchanneloutputcard.cpp \
        vch003/channels/outchanneloutputcard.cpp \
        vch003/data/vch003useractivity.cpp \
        vch003/database/vch003tables.cpp \
        vch003/log/measurementsmodel.cpp \
        vch003/log/reclistmodel.cpp \
        vch003/log/abstracteventlogmodel.cpp \
        vch003/log/abstractmeaslogmodel.cpp \
        vch003/log/activeeventmodel.cpp \
        vch003/log/currentmeasurementsmodel.cpp \
        vch003/log/eventlogmodel.cpp \
        vch003/log/hiddeneventmodel.cpp \
        vch003/log/measlogmodel.cpp \
        vch003/log/useractivitylogmodel.cpp \
        vch003/monitorring/currentrequest.cpp \
        vch003/monitorring/responsehandlers/allresponsehandlers.cpp \
        vch003/monitorring/responsehandlers/authorizationresponsehandler.cpp \
        vch003/monitorring/responsehandlers/deviceresponsehandler.cpp \
        vch003/monitorring/responsehandlers/logresponsehandler.cpp \
        vch003/monitorring/responsehandlers/responsehandler.cpp \
        vch003/monitorring/responsehandlers/usercontrolresponsehandler.cpp \
        vch003/monitorring/filerequest.cpp \
        vch003/monitorring/request.cpp \
        vch003/monitorring/tcpreader.cpp \
        vch003/monitorring/vch003monitoring.cpp \
        vch003/monitorring/vch003requests.cpp \
        vch003/vch003.cpp \
        vch003/vch003device.cpp \
        vch003/configuration/vch003cardcreators.cpp \
        vch003/configuration/vch003options.cpp \
        main.cpp \
        ssu.cpp

HEADERS += \
    backup/backup.h \
    backup/backuplistmodel.h \
    csettings.h \
    data/datacontainer.h \
    data/measdata.h \
    database/databaseexecutor.h \
    database/database.h \
    database/table.h \
    device/configuration/channeldescription.h \
    device/configuration/ssuslot.h \
    deviceoptionsmapper.h \
    global.h \
    log/abstractdevicelogmodel.h \
    log/eventdescription.h \
    log/eventdescriptionfilterproxymodel.h \
    log/eventdescriptionlistmodel.h \
    maskdata/abstractmasklistmodel.h \
    maskdata/concatenatemasklistmodel.h \
    maskdata/maskdata.h \
    maskdata/maskrecommendation.h \
    device/configuration/connectionsettings.h \
    device/configuration/lanconnection.h \
    device/panel/card.h \
    device/panel/cardcreator.h \
    device/panel/cardlist.h \
    device/panel/panel.h \
    device/panel/cardfactory.h \
    device/panel/channel.h \
    device/configuration/deviceoptions.h \
    device/configuration/slotconfiguration.h \
    device/configuration/ssupanelconfiguration.h \
    device/configuration/userrights.h \
    device/device.h \
    device/led/led.h \
    device/led/ledlist.h \
    controller.h \
    device/user/user.h \
    device/user/userlist.h \
    maskdata/measmasklist.h \
    maskdata/thresholdlist.h \
    models/filelistmodel.h \
    programdatamanagementmodel.h \
    vch003/cards/gnss/gnssmodule.h \
    vch003/cards/timeprotocol/ethernetport.h \
    vch003/cards/timeprotocol/networkconfiguration.h \
    vch003/cards/timeprotocol/ntp.h \
    vch003/cards/timeprotocol/portstatistics.h \
    vch003/cards/timeprotocol/ptp.h \
    vch003/cards/vch003card.h \
    vch003/channels/inchannelgnsscard.h \
    vch003/data/vch003event.h \
    vch003/cards/all.h \
    vch003/cards/distributioncard.h \
    vch003/cards/generatorcard.h \
    vch003/cards/gnss/satellite.h \
    vch003/cards/gnss/satellitelistmodel.h \
    vch003/cards/gnsscard.h \
    vch003/cards/inputcard.h \
    vch003/cards/monitoringcard.h \
    vch003/cards/outputcard.h \
    vch003/cards/powercard.h \
    vch003/channels/inchannelgeneratorcard.h \
    vch003/channels/inchannelinputcard.h \
    vch003/channels/inchanneloutputcard.h \
    vch003/channels/outchanneloutputcard.h \
    vch003/data/vch003useractivity.h \
    vch003/database/vch003tables.h \
    vch003/log/measurementsmodel.h \
    vch003/log/reclistmodel.h \
    vch003/log/abstracteventlogmodel.h \
    vch003/log/abstractmeaslogmodel.h \
    vch003/log/activeeventmodel.h \
    vch003/log/currentmeasurementsmodel.h \
    vch003/log/eventlogmodel.h \
    vch003/log/hiddeneventmodel.h \
    vch003/log/measlogmodel.h \
    vch003/log/useractivitylogmodel.h \
    vch003/monitorring/currentrequest.h \
    vch003/monitorring/responsehandlers/allresponsehandlers.h \
    vch003/monitorring/responsehandlers/authorizationresponsehandler.h \
    vch003/monitorring/responsehandlers/deviceresponsehandler.h \
    vch003/monitorring/responsehandlers/logresponsehandler.h \
    vch003/monitorring/responsehandlers/responsehandler.h \
    vch003/monitorring/responsehandlers/usercontrolresponsehandler.h \
    vch003/monitorring/filerequest.h \
    vch003/monitorring/request.h \
    vch003/monitorring/tcpreader.h \
    vch003/monitorring/vch003monitoring.h \
    vch003/monitorring/vch003requests.h \
    vch003/vch003.h \
    vch003/configuration/vch003cardcreators.h \
    vch003/configuration/vch003options.h \
    vch003/configuration/vch003userrights.h \
    vch003/vch003device.h \
    ssu.h

RESOURCES += qml.qrc \
    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD/view
QML_IMPORT_PATH += $$PWD/view/device

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include("$$PWD/../../../QuickCustomControls/qcc.pri")


contains(DEFINES, ENABLE_APP_UPDATER) {
AppUpdaterPath = $$PWD/../../../Updater/AppUpdater
include($${AppUpdaterPath}/AppUpdater.pri)
include($${AppUpdaterPath}/updatewidgets/UpdateWidget.pri)
#include($${AppUpdaterPath}/updatewidgetsQml/UpdateWidget.pri)
}

contains(DEFINES, DEVICE_TESTING_FORMS) {
#HEADERS += testing/devicetesting.h \
#        testing/testlistmodel.h
#SOURCES += testing/devicetesting.cpp \
#        testing/testlistmodel.cpp
}

contains(DEFINES, DEBUG_REDIRECT_TO_FILE) {
HEADERS += installmessagehandler.h
SOURCES += installmessagehandler.cpp
}


TRANSLATIONS = ts/ssumanager_ru.ts
RC_FILE += icon.rc



lupdate_only {
    SOURCES += view/*.qml \
view/controls/*.qml \
view/controls/table/*.qml \
view/controls/chart/*.qml \
view/device/*.qml \
view/dialogs/*.qml \
view/menu/*.qml \
view/menu/connection/*.qml \
view/menu/mask/*.qml \
view/vch003/menu/measurement/*.qml \
view/strings/*.js \
view/theme/*.qml \
view/vch003/*.qml \
view/vch003/cards/*.qml \
view/vch003/menu/events/*.qml \
view/vch003/menu/*.qml \
view/vch003/pages/*.qml \
view/vch003/pages/gnss/*.qml \
view/vch003/pages/timeprotocol/*.qml \
view/vch003/pages/private/*.js \
view/vch003/pages/private/*.qml
}




