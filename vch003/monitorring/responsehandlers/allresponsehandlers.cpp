#include "allresponsehandlers.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "vch003/cards/vch003card.h"
#include "vch003/monitorring/vch003requests.h"
#include "vch003/vch003.h"
#include "vch003/vch003device.h"
#include "vch003/configuration/vch003options.h"

#include <QtMath>
#include "csettings.h"
#include "vch003/cards/gnsscard.h"

//-----------------------------------------------------------------------------------------------------

namespace ssu {

    TrapParseHandler::TrapParseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool TrapParseHandler::handle(const QString &data)
    {
        QRegularExpression re("TRAP, (A\\d{2}), (Card[^,]+), ([^,]*), (\\d{1,4}),", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = re.match(data);
        if(match.hasMatch()) {
            // пришел трап..
            auto slot = match.captured(1);
            SSU::CardType type = vch003request::cardTypeFromString(match.capturedRef(2));
            bool isok;
            int eventId = match.capturedRef(4).toInt(&isok);
            if(!isok) eventId = -1;
            bool atEnd = data.contains("END", Qt::CaseInsensitive);
            analyzeTrap(slot, type, eventId, atEnd);
        }
        else
            return nextHandle(data);
        return true;
    }

    void TrapParseHandler::analyzeTrap(const QString &slot, SSU::CardType type, int eventId, bool atEnd)
    {
        Q_UNUSED(atEnd)

        bool getInve = false;
        bool paramUpdate = true;
        bool eventLogUpdate = true;
        bool activeEventsUpdate = true;

        auto vch003 = qobject_cast<Vch003Device*>(device_);

        switch (static_cast<Vch003::DeviceEvent>(eventId)) {
            case Vch003::DeviceEvent::CardIn:
            case Vch003::DeviceEvent::CartOut:
                // событие появления платы или изъятие
                getInve = true;
                paramUpdate = false;
                break;
                //_________________________________________________________ Активные события
            case Vch003::DeviceEvent::DisqualificationBySSM:         // Входная плата
            case Vch003::DeviceEvent::DisqualificationByStatus:
            case Vch003::DeviceEvent::LossOfSignal:
            case Vch003::DeviceEvent::LossOfFrame:
            case Vch003::DeviceEvent::LossOfMultiframe:
            case Vch003::DeviceEvent::SignalDelay:

            case Vch003::DeviceEvent::ErrTransmEnd:
            case Vch003::DeviceEvent::ErrCRC:
            case Vch003::DeviceEvent::ErrHDB3:
            case Vch003::DeviceEvent::ErrFreqCapt:

            case Vch003::DeviceEvent::ActiveGeneratorFailure:        // Выходная плата
            case Vch003::DeviceEvent::BackupGeneratorFailure:
            case Vch003::DeviceEvent::DisqualificationInternalUSS:
            case Vch003::DeviceEvent::BackupChannelNoSync:
            case Vch003::DeviceEvent::BothChanNoSync:
            case Vch003::DeviceEvent::Sync500Fault:
            case Vch003::DeviceEvent::Bypass:
            case Vch003::DeviceEvent::NoMeasMsgs:
                // if(!atEnd) eventLogUpdate = false;  // В начале не нужно считывать лог..
                break;

            case Vch003::DeviceEvent::DisqualificationByMTIE:
            case Vch003::DeviceEvent::DisqualificationByTDEV:
            case Vch003::DeviceEvent::DisqualificationByDff:

            case Vch003::DeviceEvent::NoActiveCard:
            case Vch003::DeviceEvent::WrongSlot:
            case Vch003::DeviceEvent::CardNotFullyInserted:
                paramUpdate = false;    // не нужно
                //@N считать лог измерений в будущем..
                // if(!atEnd) eventLogUpdate = false;  // В начале не нужно считывать лог..
                break;

            case Vch003::DeviceEvent::Warmup:                        // Плата генератора
            case Vch003::DeviceEvent::Freerun:
            case Vch003::DeviceEvent::Holdover:
            case Vch003::DeviceEvent::Adjustment:
            case Vch003::DeviceEvent::GeneratorERROR:
                // if(!atEnd) eventLogUpdate = false;  // В начале не нужно считывать лог..
                break;
            case Vch003::DeviceEvent::Tracking:      // Это по сути информационное событие. Но оно закрывает другие
                // считать все!!
                break;

            case Vch003::DeviceEvent::ActiveSignalFailure:
            case Vch003::DeviceEvent::GeneratorFailure:
            case Vch003::DeviceEvent::DDSFailure:
            case Vch003::DeviceEvent::MeasurmentSystemFailure:
            case Vch003::DeviceEvent::ControlLoopFailure:

            case Vch003::DeviceEvent::PowerAFailure:             // БМ
            case Vch003::DeviceEvent::PowerBFailure:
            case Vch003::DeviceEvent::FuseAfail:
            case Vch003::DeviceEvent::FuseBfail:
            case Vch003::DeviceEvent::PowerAout:
            case Vch003::DeviceEvent::PowerBout:
                // if(!atEnd) eventLogUpdate = false;  // В начале не нужно считывать лог..
                break;

            case Vch003::DeviceEvent::EventLogOverflow:
            case Vch003::DeviceEvent::MeasurementLogOverflow:
            case Vch003::DeviceEvent::UserActivityLogOverflow:
                // if(!atEnd) eventLogUpdate = false;  // В начале не нужно считывать лог..
                paramUpdate = false;    // не нужно
                break;


                //_________________________________________________________ Информационные события!
            case Vch003::DeviceEvent::ChangedToPassive:
            case Vch003::DeviceEvent::ChangedToActive:
            case Vch003::DeviceEvent::ActiveInputChanged:

            case Vch003::DeviceEvent::SignalAppear:
            case Vch003::DeviceEvent::InpSSMChanged:
            case Vch003::DeviceEvent::InpTypChanged:

            case Vch003::DeviceEvent::BackupInputChanged:
            case Vch003::DeviceEvent::OutSSMmodeChanged:
            case Vch003::DeviceEvent::OutSSMChanged:
            case Vch003::DeviceEvent::OutManInpChanged:
            case Vch003::DeviceEvent::OutTypChanged:


            case Vch003::DeviceEvent::GModeChanged:
            case Vch003::DeviceEvent::GenManInpChanged:
                activeEventsUpdate = false; // Не нужно считывать лог аварий
                break;

            case Vch003::DeviceEvent::ConfigChanged: {
                paramUpdate = false;
                activeEventsUpdate = false;

                auto cmd = new vch003request::GetPanelConfiguration;   // изменена конфигурация плат. Нужно бы перечитать ее
                cmd->setDelay(SSU::kDelayForSendingRequest);
                vch003->send(cmd);

                // Пока что решили, что не нужно. Тк все равно плата перезагружается
                eventLogUpdate = false;
                break;
            }
                //_________________________________________________________ разобраться когда понадобится!!
            case Vch003::DeviceEvent::PriorityTableError:
            case Vch003::DeviceEvent::ConnectionLimitExceeded:
                paramUpdate = false;
                break;

            default: break; // для остальных событий пока что просто считаем все..
        }

        //        auto it = device_->eventDescriptionList()->constFind(static_cast<int>(type), eventId);
        //        if(it == device_->eventDescriptionList()->constEnd()) {
        //            device_->trap(slot, "Unknown event №" + QString::number(eventId), SSU::EventStatus::NotAlarm, atEnd);
        //        }
        //        else {
        //            device_->trap(slot, it->description(), it->status(), atEnd);
        //        }

        if(getInve) {
            auto cmd = new vch003request::GetInventory;
            cmd->setDelay(SSU::kDelayForSendingRequest);
            vch003->send(cmd);
        }

        if(CSettings::getInstance()->updatingCardParametersOnEventEnabled() && paramUpdate && type != SSU::CardType::InvalidType) {
            if(device_->panel()->inventory()->contains(slot)) {
                auto cmd = new vch003request::GetCardParameters(slot);
                cmd->setDelay(SSU::kDelayForSendingRequest);
                vch003->send(cmd);
            }
        }

        // считаем активные события
        if(activeEventsUpdate) {
            auto cmd = new vch003request::GetActiveEvents;
            cmd->setDelay(SSU::kDelayForSendingRequest);
            vch003->send(cmd);
        }

        if(CSettings::getInstance()->updatingLogsOnEventEnabled() && eventLogUpdate) {// считаем лог начиная с последнего момента или за последние сутки..
            auto cmd = new vch003request::GetEventLog(device_);
            cmd->setDelay(SSU::kDelayForSendingRequest);
            vch003->send(cmd);
        }

        qInfo() << "analyzeTrap" << slot << type << eventId << atEnd << ":" << getInve << paramUpdate << activeEventsUpdate;
    }



    //-----------------------------------------------------------------------------------------------------

    GetCurrentUserInfoResponseHandler::GetCurrentUserInfoResponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetCurrentUserInfoResponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetCurrentUserInfo::cmd(), data, slot, command, request)) {
            QRegularExpression re("[^,]+, [^,]+, (" + vch003request::userLevelToString(SSU::UserLevel::User) + "|" +
                                  vch003request::userLevelToString(SSU::UserLevel::Admin) + "|" +
                                  vch003request::userLevelToString(SSU::UserLevel::Security) + ")", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(request);
            if(match.hasMatch()) {
                auto level = vch003request::userLevelFromString(match.capturedRef(1));
                device_->connectionSettings()->setLevel(level);
            }
        }
        else
            return nextHandle(data);
        return true;
    }


    //-----------------------------------------------------------------------------------------------------

    InventoryResponseHandler::InventoryResponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool InventoryResponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetInventory::cmd(), data, slot, command, request)) {
            QList <CardInfo> infoList;
            QRegularExpression inveRe("(A\\d\\d), (Card[^,]+), SN ([^,]*), SW ([^,]*), HW (.*)$", QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match;

            QTextStream stream(&request, QIODevice::ReadOnly);

            QString line;
            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = inveRe.match(line);
                if(!match.hasMatch()) continue;

                int address = SSU::addressFromString(match.capturedRef(1));
                SSU::CardType type = vch003request::cardTypeFromString(match.capturedRef(2));
                QString sn = match.captured(3);
                QString sw = match.captured(4);
                QString hw = match.captured(5);

                if(address == -1) continue;
                infoList.append({ type, address, sn, sw, hw });
            }

            Vch003Device* vch003 = qobject_cast<Vch003Device*>(device_);

            QList <Card *> updatedCardList;
            auto inventory = device_->panel()->inventory();

            if(inventory->update(infoList, &updatedCardList)) {
                bool BMinput = false;   // была ли вставлена плата мониторинга..

                // Восстановим информацию о девайсе и его данные за сутки
                for(auto card : updatedCardList) {      // отправим команды с инициализацией платы
                    if(card->type() == SSU::CardType::Monitoring) {
                        BMinput = true;  // да, вставлена..
                    }

                    for(auto &cmd : static_cast<Vch003Card*>(card)->initCommandList()) {
                        vch003->send(cmd);
                    }
                }

                if(BMinput) {   // была вставлена плата мониторинга..
                    vch003->updateActiveEvents();           // Считать активные события
                    vch003->updateEventDescription();       // Считать список возможных событий
                    vch003->updateCurrentMeas();            // Считать текущие измерения
                    vch003->updateThresholds();             // Считать пороговые значения

                    vch003->updateUserList();               // Считать список пользователей

                    // Загрузим истории
                    if(CSettings::getInstance()->updatingLogsOnEventEnabled())
                        vch003->downloadEventLog();

                    if(CSettings::getInstance()->userLogPollInterval() > 0)
                        vch003->downloadUserLog();

                    if(CSettings::getInstance()->measLogPollInterval() > 0)
                        vch003->downloadReclist();
                }

                // Составим список периодического опроса
                if(inventory->isEmpty()) {
                    // это какая-то ошибка.. Такого не может быть..
                    // будем повторять, пока не найдем..
                    auto getInventoryRequest = new vch003request::GetInventory;
                    getInventoryRequest->setDelay(CSettings::getInstance()->pollingInterval());
                    vch003->monitoring()->updatePollQueue({ getInventoryRequest });  // Будем опрашивать список инвентаризации
                }
                else {
                    // Составим список опроса устройства если он нужен..
                    QList <Request*> reqList;

                    auto getInventoryRequest = new vch003request::GetInventory;
                    getInventoryRequest->setDelay(std::max(CSettings::getInstance()->pollingInterval() * 4, 30000));   // пусть иногда опрашивает все же..
                    reqList.append(getInventoryRequest);

                    if(CSettings::getInstance()->pollingInterval() > 0) {
                        // Считывание параметров платы
                        for(auto it = inventory->constBegin(); it != inventory->constEnd(); ++it) {
                            reqList.append(static_cast<Vch003Card*>(*it)->requestList());
                        }
                    }

                    // Для периодического опроса нам также нужно..

                    if(CSettings::getInstance()->userLogPollInterval() > 0) {
                        // лог действий пользователя..
                        if(inventory->monitoringCard() != nullptr) {    // есть плата мониторинга
                            // Обновления лога пользователей
                            auto getUserLog = new vch003request::GetUserLog(device_);
                            getUserLog->setDelay(CSettings::getInstance()->userLogPollInterval());
                            reqList.append(getUserLog);
                        }
                    }

                    // Измерения
                    if(inventory->findCard(SSU::Output)) {
                        // посмотрим есть ли в данный момент плата выходных сигналов? Если есть, то нужно считывать измерения периодически..
                        if(CSettings::getInstance()->measLogPollInterval() > 0) {
                            auto getReclist = new vch003request::GetReclist(device_);
                            getReclist->setDelay(CSettings::getInstance()->measLogPollInterval());
                            reqList.append(getReclist);
                        }

                        // Обновления текущих измерений
                        if(CSettings::getInstance()->measDataPollInterval() > 0) {
                            auto getCurrentMeas = new vch003request::GetCurrentMeas;
                            getCurrentMeas->setDelay(CSettings::getInstance()->measDataPollInterval());
                            reqList.append(getCurrentMeas);
                        }
                    }

                    vch003->monitoring()->updatePollQueue(reqList);
                }
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetParametersReponseHandler::GetParametersReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetParametersReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetCardParameters::cmd(), data, slot, command, request)) {
            auto inventory = device_->panel()->inventory();

            qDebug() << "GetParametersReponseHandler::handle 1 test" << inventory;
            Card *card = slot.isEmpty() ? inventory->monitoringCard()               // значит пришло от платы мониторинга..
                                        : inventory->card(slot);
            if(card != nullptr) {
                qDebug() << "GetParam request" << request;
                static_cast<Vch003Card*>(card)->parseParameters(request);
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetSatelliteListReponseHandler::GetSatelliteListReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetSatelliteListReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetSatelliteList::cmd(), data, slot, command, request)) {
            auto inventory = device_->panel()->inventory();
            Card *card = slot.isEmpty() ? nullptr
                                        : inventory->card(slot);
            if(card != nullptr && card->type() == SSU::Gnss) {
                QList <Satellite> satellites;

                QRegularExpression re("(\\d+), (\\d+), (-?\\d+\\.?\\d*), (-?\\d+\\.?\\d*), (\\d+\\.?\\d*), (\\d+)");
                QRegularExpressionMatch match;
                bool isok;

                QTextStream stream(&request, QIODevice::ReadOnly);
                QString line;
                while(stream.readLineInto(&line)) {
                    if(line.isEmpty())
                        continue;

                    match = re.match(line);
                    if(!match.hasMatch()) continue;

                    Satellite sat;

                    sat.id = match.capturedRef(1).toInt(&isok);
                    if(!isok) continue;

                    sat.type = static_cast<int>(vch003request::satelliteSystemFromIndex(match.capturedRef(2).toInt(&isok)));
                    if(!isok) continue;

                    sat.elevation = match.capturedRef(3).toDouble(&isok);
                    if(!isok) continue;

                    sat.azimuth = match.capturedRef(4).toDouble(&isok);
                    if(!isok) continue;

                    sat.snr = match.capturedRef(5).toDouble(&isok);
                    if(!isok) continue;

                    sat.tracked = match.capturedRef(6).toInt(&isok);
                    if(!isok) continue;

                    satellites.append(sat);
                }

                static_cast<GnssCard*>(card)->satelliteList()->set(satellites);
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetGnssPortSettingsReponseHandler::GetGnssPortSettingsReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetGnssPortSettingsReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetGnssPortSettings::cmd() + "\\d+", data, slot, command, request)) {
            auto inventory = device_->panel()->inventory();
            Card *card = slot.isEmpty() ? nullptr
                                        : inventory->card(slot);
            if(card != nullptr && card->type() == SSU::Gnss) {
                bool isok;
                // посмотрим сколько дат в сообщении
                QRegularExpression re(vch003request::GetGnssPortSettings::cmd() + "(\\d+)");
                auto match = re.match(command);
                if(!match.hasMatch())
                    return true;
                auto portIndex = match.capturedRef(1).toInt(&isok);
                if(isok) {
                    static_cast<GnssCard*>(card)->parsePortSettings(portIndex, request);
                }
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetGnssPortStatisticsReponseHandler::GetGnssPortStatisticsReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetGnssPortStatisticsReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetGnssPortStatistics::cmd() + "\\d+", data, slot, command, request)) {
            auto inventory = device_->panel()->inventory();
            Card *card = slot.isEmpty() ? nullptr
                                        : inventory->card(slot);
            if(card != nullptr && card->type() == SSU::Gnss) {
                bool isok;
                // посмотрим сколько дат в сообщении
                QRegularExpression re(vch003request::GetGnssPortStatistics::cmd() + "(\\d+)");
                auto match = re.match(command);
                if(!match.hasMatch())
                    return true;
                auto portIndex = match.capturedRef(1).toInt(&isok);
                if(isok) {
                    static_cast<GnssCard*>(card)->parsePortStatistics(portIndex, request);
                }
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetGnssNetworkConfigurationReponseHandler::GetGnssNetworkConfigurationReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetGnssNetworkConfigurationReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetGnssNetworkConfiguration::cmd() + "\\d+", data, slot, command, request)) {
            auto inventory = device_->panel()->inventory();
            Card *card = slot.isEmpty() ? nullptr
                                        : inventory->card(slot);
            if(card != nullptr && card->type() == SSU::Gnss) {
                bool isok;
                // посмотрим сколько дат в сообщении
                QRegularExpression re(vch003request::GetGnssNetworkConfiguration::cmd() + "(\\d+) (\\d+)");
                auto match = re.match(command);
                if(!match.hasMatch())
                    return true;
                auto portIndex = match.capturedRef(1).toInt(&isok);
                if(isok) {
                    auto confIndex = match.capturedRef(2).toInt(&isok);
                    if(isok) {
                        static_cast<GnssCard*>(card)->parseNetworkConfiguration(portIndex, confIndex, request);
                    }
                }
            }
        }
        else
            return nextHandle(data);
        return true;
    }









    //-----------------------------------------------------------------------------------------------------

    GetPanelConfigReponseHandler::GetPanelConfigReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetPanelConfigReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetPanelConfiguration::cmd(), data, slot, command, request)) {
            QList <CardInfo> infoList;
            QRegularExpression inveRe("(A\\d\\d), (Card[^,]+)", QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match;

            QTextStream stream(&request, QIODevice::ReadOnly);

            QString line;

            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = inveRe.match(line);
                if(!match.hasMatch()) continue;

                int address = SSU::addressFromString(match.capturedRef(1));
                SSU::CardType type = vch003request::cardTypeFromString(match.capturedRef(2));

                if(address == -1 || type == SSU::CardType::InvalidType) continue;
                infoList.append({ type, address, "", "", "" });
            }

            //@N переделать бы это.. Нужно вообще все считывать с устройства по сути
            bool isok = false;
            auto conf = device_->panel()->panelConfiguration();
            for(int i = 0; i < conf->size(); ++i) {
                isok = false;
                for(int j = 0; j < infoList.size(); ++j) {
                    if(conf->at(i)->address() == infoList.at(j).address) {
                        isok = true;
                        conf->at(i)->setAvailable(true);
                        conf->at(i)->setType(infoList.at(j).type);
                        break;
                    }
                }
                if(!isok)
                    conf->at(i)->setAvailable(false);
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    MeasResetReponseHandler::MeasResetReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool MeasResetReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::ResetMeasurements::cmd(), data, slot, command, request)) {
            auto rec = new vch003request::GetCurrentMeas;
            rec->setDelay(SSU::kDelayBeforeCheckRequest);
            qobject_cast<Vch003Device*>(device_)->send(rec);
            //        qobject_cast<Vch003Device*>(device_)->updateCurrentMeas();
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    SetAllThresholdsReponseHandler::SetAllThresholdsReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool SetAllThresholdsReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::SetAllThreshols::cmd(), data, slot, command, request)) {
            qobject_cast<Vch003Device*>(device_)->send(new vch003request::GetThresholds);
        }
        else
            return nextHandle(data);
        return true;
    }

    //-----------------------------------------------------------------------------------------------------

    GetThresholdReponseHandler::GetThresholdReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetThresholdReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetThresholds::cmd(), data, slot, command, request)) {
            QRegularExpression re("(\\d+), (MTIE|TDEV|df_F)_thr, (((" + REGEXP_FLOAT + "|-)[, ]*){" + QString::number(device_->options()->kMeasIntervals.size()) + "})", QRegularExpression::CaseInsensitiveOption);
            QHash <int, DataContainer> thresholds;

            QRegularExpressionMatch match;
            bool isok;

            QTextStream stream(&request, QIODevice::ReadOnly);
            QString line;

            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = re.match(line);
                if(!match.hasMatch()) continue;

                int channel = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(channel < 0 || channel >= Vch003DeviceOptions::kNumberOfOutputCardInChannels) continue;    // проверка от всякой фигни..

                auto dataType = vch003request::measDataTypeFromString(match.capturedRef(2));
                if(dataType == SSU::MeasDataType::Invalid) continue;
                double coef = vch003request::getThresholdCoef(dataType);

                QList <double> list;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                auto values = match.capturedRef(3).split(", ", QString::SkipEmptyParts);
#else
                auto values = match.capturedRef(3).split(", ", Qt::SkipEmptyParts);
#endif

                if(values.size() != device_->options()->kMeasIntervals.size()) continue;  // должно быть все норм..
                for(int i = 0; i < device_->options()->kMeasIntervals.size(); ++i) {
                    //                    qInfo() << "--------" << values.at(i) << (values.at(i) == "-") << values.at(i).toInt() << (values.at(i).toInt() == 0);
                    if(values.at(i) == "-") {
                        list.append(NAN);
                    }
                    else {
                        int value = values.at(i).toInt(&isok);
                        if(!isok || value == 0) {    // Сравним с 0
                            list.append(NAN);
                        }
                        else {
                            list.append(value * coef);
                        }
                    }
                }

                auto it = thresholds.find(channel);
                if(it == thresholds.end()) {
                    DataContainer d;
                    d.set(static_cast<int>(dataType), list);
                    thresholds.insert(channel, d);
                }
                else {
                    it.value().set(static_cast<int>(dataType), list);
                }
            }

            for(int i = 0; i < Vch003DeviceOptions::inputLinkedChannels.size(); ++i) {
                if(Vch003DeviceOptions::inputLinkedChannels.at(i).isEmpty())    // перестраховка..
                    continue;
                const int channel =  Vch003DeviceOptions::inputLinkedChannels.at(i).first();    // Найдем первый канал. Второй должен быть такой же.. Он нам не нужен
                auto it = thresholds.constFind(channel);
                if(it == thresholds.constEnd())
                    continue;
                device_->thresholdList()->setMaskData(i, it.value());
            }
        }
        else
            return nextHandle(data);
        return true;
    }


    //-----------------------------------------------------------------------------------------------------

    ResetThresholdsReponseHandler::ResetThresholdsReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool ResetThresholdsReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::ResetThresholds::cmd(), data, slot, command, request)) {
            auto rec = new vch003request::GetThresholds;
            rec->setDelay(SSU::kDelayBeforeCheckRequest);
            qobject_cast<Vch003Device*>(device_)->send(rec);
        }
        else
            return nextHandle(data);
        return true;
    }




}
