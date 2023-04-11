#include "logresponsehandler.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "vch003/monitorring/vch003requests.h"
#include "vch003/vch003.h"
#include "vch003/vch003device.h"
#include "vch003/configuration/vch003options.h"

#include <QtMath>

#include "vch003/data/vch003event.h"
#include "vch003/data/vch003useractivity.h"
#include "vch003/database/vch003tables.h"

namespace ssu {
    GetEventDescriptionReponseHandler::GetEventDescriptionReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetEventDescriptionReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetEventDescription::cmd(), data, slot, command, request)) {
            QRegularExpression re("(Card.+)");

            auto match = re.match(command);
            if(!match.hasMatch())
                return false;

            auto cardType = vch003request::cardTypeFromString(match.capturedRef(1));
            if(cardType == SSU::CardType::InvalidType)
                return false;

            re.setPattern("(\\d{1,3}), ([^,]*), (.+)");
            bool isok;

            QList <EventDescription> events;

            QTextStream stream(&request, QIODevice::ReadOnly);
            QString line;
            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = re.match(line);
                if(!match.hasMatch()) continue;
                int eventId = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                QString description = match.captured(2);
                SSU::EventStatus status = vch003request::eventStatusFromString(match.capturedRef(3));
                events.append({ cardType, eventId, description, status });
            }

            device_->eventDescriptionList()->update(events);
        }
        else
            return nextHandle(data);
        return true;
    }

    GetActiveEventsReponseHandler::GetActiveEventsReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetActiveEventsReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetActiveEvents::cmd(), data, slot, command, request)) {
            qInfo() << "GetActiveEventsReponseHandler::handle start";

            QList <Vch003Event> currentEvents;   //@N пока что так. Мб придет в голову что-то другое
            QRegularExpression re("(" + REGEXP_TIME + "), (A\\d{2}), (Card[^,]+), ([^,]*), (\\d{1,3}), ([^,\\r\\n]+)(, chan (\\d{1,3}))?");
            //            re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match;
            bool isok;

            QTextStream stream(&request, QIODevice::ReadOnly);
            QString line;
            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = re.match(line);
                if(!match.hasMatch()) continue;

                auto occTime = QDateTime::fromString(match.captured(1), DEVICE_TIME_FORMAT);
                if(!occTime.isValid()) continue;
                occTime.setTimeSpec(Qt::UTC);

                Vch003Event event;

                event.occurredTime = occTime.toSecsSinceEpoch();
                event.slot = match.captured(2).remove(QRegExp("\\s"));
                if(event.slot.isEmpty()) continue;

                event.card = vch003request::cardTypeFromString(match.capturedRef(3));

                event.eventId = match.capturedRef(5).toInt(&isok);
                if(!isok) continue;

                auto channel = match.capturedRef(8);
                if(!channel.isEmpty())
                    event.channel = channel.toInt();

                currentEvents.append(event);
            }

            // 1) Нужно бы пройтись и посмотреть. Нужно ли перезаписывать данные базе
            // 2) Если нужно, то нужно посмотреть были ли там комментарии
            // 3) Если что-то пропало и там есть коммент, то нужно записать в таблицу истории событий

            bool updateActiveEventsTable = false;
            auto activeEventTable = static_cast<Vch003ActiveEventsTable*>(device_->databaseExecutor()->table(static_cast<int>(Vch003::DatabaseTable::ActiveEvents)));

            if(activeEventTable != nullptr) {
                auto prevActiveEvents = activeEventTable->select(device_->database(), {}, &isok);

                if(isok) {
                    QVariantList eventsToSave;   // Эти события нужно записать в базу истории событий
                    updateActiveEventsTable = prevActiveEvents.size() != currentEvents.size();

                    if(currentEvents.isEmpty()) {
                        // Нужно все очистить. Но сначала посмотрим есть ли события с комментариями. Если есть, то запомним
                        for(const auto &event : prevActiveEvents) {
                            if(!event.comment.isEmpty())
                                eventsToSave.append(QVariant::fromValue(event));
                        }
                    }
                    else {
                        // Нужно пройтись и сравнить что было и что сейчас
                        for(const auto &event : prevActiveEvents) {
                            int index = currentEvents.indexOf(event);
                            // есть ли в новом списке старый элемент

                            if(index == -1) {
                                // что-то точно изменилось..
                                updateActiveEventsTable = true;

                                // похоже пропал.. Нужно запомнить его.. если есть коммент
                                if(!event.comment.isEmpty())
                                    eventsToSave.append(QVariant::fromValue(event));
                            }
                            else {
                                // Если есть коммент, то оставим его
                                if(!currentEvents.at(index).comment.isEmpty())
                                    currentEvents[index].comment = event.comment;
                                currentEvents[index].readed = event.readed;
                            }
                        }
                    }

                    if(!eventsToSave.isEmpty()) {
                        qInfo() << "GetActiveEventsReponseHandler. Move event to log" << eventsToSave.size();
                        device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::EventLog), eventsToSave);   // to DB
                        // Тут можно не обновлять метку времени.. Тк все равно последняя дата не изменится
                    }
                }
                else updateActiveEventsTable = true;
            }
            else updateActiveEventsTable = true;

            qInfo() << "GetActiveEventsReponseHandler::" << updateActiveEventsTable << currentEvents.size();

            // Если нужно, то обновим список событий
            if(updateActiveEventsTable) {
                QVariantList currentEventsVarList;
                for(const auto &event : currentEvents) {
                    currentEventsVarList.append(QVariant::fromValue(event));
                }
                device_->databaseExecutor()->updateRows(static_cast<int>(Vch003::DatabaseTable::ActiveEvents), currentEventsVarList);   // to DB
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    GetEventLogReponseHandler::GetEventLogReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetEventLogReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetEventLog::cmd(), data, slot, command, request)) {
            QVariantList log;   // QList <Event> log;   //@N пока что так. Мб придет в голову что-то другое

            qInfo() << "GetEventLogReponseHandler::handle start";

            QRegularExpression re("(\\d+), (\\d+), (A\\d{2}), (\\d{1,2}), (\\d{1,3}), (-1|\\d{1,3}), (-|.+)");
            QRegularExpressionMatch match;

            bool isok;
            int count = 0;  // Кол-во записанных строк в базу

            QTextStream stream(&request, QIODevice::ReadOnly);
            QString line;
            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = re.match(line);
                if(!match.hasMatch()) continue;

                Vch003Event event;

                event.occurredTime = match.capturedRef(1).toLongLong(&isok);
                if(!isok) continue;

                event.clearedTime = match.capturedRef(2).toLongLong(&isok);
                if(!isok) continue;

                event.slot = match.captured(3).remove(QRegExp("\\s"));
                if(event.slot.isEmpty()) continue;

                int typeIndex = match.capturedRef(4).toInt(&isok);
                if(!isok) continue;

                event.card = vch003request::cardTypeFromIndex(typeIndex);

                event.eventId = match.capturedRef(5).toInt(&isok);
                if(!isok) continue;

                auto channel = match.capturedRef(6);
                if(!channel.isEmpty())
                    event.channel = channel.toInt();

                auto param = match.captured(7);
                event.parameters = param == "-" ? "" : param;

                log.append(QVariant::fromValue(event)); // log.append(event);

                if(log.size() >= SSU::kMaxNumberOfLinesToLoadTheDatabase) {
                    qInfo() << "GetEventLog::" << log.size() << "> MaxNumberOfLinesToLoadTheDatabase";
                    // загрузим данные в базу и продолжим разбирать..
                    count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::EventLog), log);   // to DB
                    log.clear();
                }
            }

            qInfo() << "GetEventLog::" << log.size();

            if(!log.isEmpty()) {
                count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::EventLog), log);   // to DB
            }

            if(count > 0) {
                // Обновим метку времени
                static_cast<Vch003Device*>(device_)->updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::EventLog));
            }

            qInfo() << "GetEventLogReponseHandler::handle end" << count;
        }
        else
            return nextHandle(data);
        return true;
    }


    GetUserLogReponseHandler::GetUserLogReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetUserLogReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetUserLog::cmd(), data, slot, command, request)) {
            QVariantList log;   // QList <Vch003UserActivity> log;   //@N пока что так. Мб придет в голову что-то другое

            QRegularExpression re("(\\d+), (A\\d{2}|), ([^,]+), (\\d{1,2}), (\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}), (.+)");
            QRegularExpressionMatch match;

            bool isok;
            int count = 0;  // Кол-во записанных строк в базу

            QTextStream stream(&request, QIODevice::ReadOnly);
            QString line;
            while(stream.readLineInto(&line)) {
                if(line.isEmpty())
                    continue;

                match = re.match(line);
                if(!match.hasMatch()) continue;

                Vch003UserActivity activity;
                activity.occurredTime = match.capturedRef(1).toLongLong(&isok);
                if(!isok) continue;
                activity.username = match.captured(3).remove(QRegExp("\\s"));
                activity.address = match.captured(5).remove(QRegExp("\\s"));
                activity.action = match.captured(6);

                log.append(QVariant::fromValue(activity)); // log.append(activity);

                if(log.size() >= SSU::kMaxNumberOfLinesToLoadTheDatabase) {
                    qInfo() << "GetUserLog::" << log.size() << "> MaxNumberOfLinesToLoadTheDatabase";
                    // загрузим данные в базу и продолжим разбирать..
                    count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::UserActivityLog), log);   // to DB
                    log.clear();
                }
            }

            qInfo() << "GetUserLog::" << log.size();

            if(!log.isEmpty()) {
                count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::UserActivityLog), log);   // to DB
            }

            if(count > 0) {
                // Обновим метку времени
                static_cast<Vch003Device*>(device_)->updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::UserActivityLog));
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    GetReclistReponseHandler::GetReclistReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetReclistReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetReclist::cmd(), data, slot, command, request)) {
            // посмотрим сколько дат в сообщении
            QRegularExpression re(vch003request::GetReclist::cmd() + " ?(" + REGEXP_TIME + ")?[ -]*(" + REGEXP_TIME + ")?");
            auto match = re.match(command);
            if(!match.hasMatch())
                return true;

            QDateTime time1 = QDateTime::fromString(match.captured(1), DEVICE_TIME_FORMAT);
            QDateTime time2 = QDateTime::fromString(match.captured(2), DEVICE_TIME_FORMAT);

            if(time1.isValid()) time1.setTimeSpec(Qt::UTC);
            else return false;  // фигня какая-то..

            if(time2.isValid()) time2.setTimeSpec(Qt::UTC);

            // найдем список сигналов
            bool isok;
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
            auto list = request.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
#else
            auto list = request.split(QRegExp("[\\r\\n]+"), Qt::SkipEmptyParts);
#endif

            auto vch003 = qobject_cast<ssu::Vch003Device*>(device_);

            for(const QString &line : list) {
                int channel = line.toInt(&isok);
                if(!isok) continue;
                if(channel < 0 || channel >= Vch003DeviceOptions::kNumberOfOutputCardInChannels)
                    continue;    // проверка от всякой фигни..

                // глянем что за команда то пришла.. С двумя датами или с одной
                if(time2.isValid()) {
                    vch003->send(new vch003request::GetMeasLog(channel, time1, time2, device_));
                }
                else {
                    // одна дата
                    vch003->send(new vch003request::GetMeasLog(channel, device_));
                }
            }
        }
        else
            return nextHandle(data);
        return true;
    }

    GetMeasLogReponseHandler::GetMeasLogReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetMeasLogReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetMeasLog::cmd(), data, slot, command, request)) {
            //@N (MTIE|TDEV|df_F) вместо числа подставить..
            QRegularExpression re("(\\d+), (\\d+), (\\d+), (((" + REGEXP_FLOAT + "|-)[, ]*){" + QString::number(device_->options()->kMeasIntervals.size()) + "})",
                                  QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match;
            bool isok;

            QList <MeasData> log;

            int count = 0;  // Кол-во записанных строк в базу
            int prevChannel = -1;
            qint64 prevTime = -1;

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

                auto dataType = vch003request::measDataTypeFromIndex(match.capturedRef(2).toInt(&isok));
                if(!isok || dataType == SSU::MeasDataType::Invalid) continue;

                qint64 time = match.capturedRef(3).toLongLong(&isok);
                if(!isok) continue;

                double coef = vch003request::getMeasCoef(dataType);
                QList <double> list;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                auto values = match.capturedRef(4).split(", ", QString::SkipEmptyParts);
#else
                auto values = match.capturedRef(4).split(", ", Qt::SkipEmptyParts);
#endif

                if(values.size() != device_->options()->kMeasIntervals.size()) continue;  // должно быть все норм..
                for(int i = 0; i < device_->options()->kMeasIntervals.size(); ++i) {
                    if(values.at(i) == "-") {
                        list.append(NAN);
                    }
                    else {
                        double value = values.at(i).toDouble(&isok);
                        if(!isok) {
                            list.append(NAN);
                        }
                        else {
                            switch (dataType) {
                                case SSU::MeasDataType::Mtie:
                                    list.append(std::max(coef, value * coef));  // 0 - не может быть..
                                    break;
                                case SSU::MeasDataType::Tdev:
                                case SSU::MeasDataType::Rf:
                                default:
                                    list.append(value * coef);
                                    break;
                            }
                        }
                    }
                }

                if(prevChannel != channel ||
                        prevTime != time) {

                    // Проверим мб пора уже что-то загрузить?
                    if(log.size() >= SSU::kMaxNumberOfLinesToLoadTheDatabase) {
                        qInfo() << "GetMeasLog::" << log.size() << "> MaxNumberOfLinesToLoadTheDatabase";
                        // загрузим данные в базу и продолжим разбирать..
                        QVariantList measurements;
                        for(const auto &meas : log) {
                            measurements.append(QVariant::fromValue(meas));
                        }
                        count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::MeasLog), measurements);   // to DB

                        log.clear();
                    }

                    // Новые данные пошли
                    MeasData meas;
                    meas.time = time;
                    meas.channel = channel;
                    log.append(meas);
                }
                // else // Продолжаются старые еще

                if(!log.isEmpty()) {
                    log.last().data.set(static_cast<int>(dataType), list);
                }

                prevChannel = channel;
                prevTime = time;
            }

            qInfo() << "GetMeasLog::" << log.size();

            if(!log.isEmpty()) {
                QVariantList measurements;
                for(const auto &meas : log) {
                    measurements.append(QVariant::fromValue(meas));
                }
                count += device_->databaseExecutor()->insertRows(static_cast<int>(Vch003::DatabaseTable::MeasLog), measurements);   // to DB
            }

            if(count > 0) {
                // Обновим метку времени
                static_cast<Vch003Device*>(device_)->updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::MeasLog));
            }

            qInfo() << "GetMeasLogReponseHandler::handle end" << count;
        }
        else return nextHandle(data);
        return true;
    }


    GetCurrentMeasReponseHandler::GetCurrentMeasReponseHandler(Device *device) : DeviceResponseHandler(device) {}

    bool GetCurrentMeasReponseHandler::handle(const QString &data)
    {
        QString slot, command, request;
        if(parseData(vch003request::GetCurrentMeas::cmd(), data, slot, command, request)) {
            QRegularExpression re("(\\d+), (MTIE|TDEV|df_F), (((" + REGEXP_FLOAT + "|-)[, ]*){" + QString::number(device_->options()->kMeasIntervals.size()) + "})",
                                  QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatch match;
            QHash <int, ssu::DataContainer> measHash;
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
                double coef = vch003request::getMeasCoef(dataType);
                QList <double> list;

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                auto values = match.capturedRef(3).split(", ", QString::SkipEmptyParts);
#else
                auto values = match.capturedRef(3).split(", ", Qt::SkipEmptyParts);
#endif

                if(values.size() != device_->options()->kMeasIntervals.size()) continue;  // должно быть все норм..
                for(int i = 0; i < device_->options()->kMeasIntervals.size(); ++i) {
                    if(values.at(i) == "-") {
                        list.append(NAN);
                    }
                    else {
                        double value = values.at(i).toDouble(&isok);
                        if(!isok) {
                            list.append(NAN);
                        }
                        else {
                            switch (dataType) {
                                case SSU::MeasDataType::Mtie:
                                    list.append(std::max(coef, value * coef));  // 0 - не может быть..
                                    break;
                                case SSU::MeasDataType::Tdev:
                                case SSU::MeasDataType::Rf:
                                default:
                                    list.append(value * coef);
                                    break;
                            }
                        }
                    }
                }

                auto it = measHash.find(channel);
                if(it == measHash.end()) {
                    DataContainer d;
                    d.set(static_cast<int>(dataType), list);
                    measHash.insert(channel, d);
                }
                else {
                    it.value().set(static_cast<int>(dataType), list);
                }
            }

            QVariantList measurements;
            for(auto it = measHash.constBegin(); it != measHash.constEnd(); ++it) {
                MeasData meas;
                meas.channel = it.key();
                meas.time = device_->currentDateTime().toSecsSinceEpoch();
                meas.data = it.value();
                measurements.append(QVariant::fromValue(meas));
            }
            device_->databaseExecutor()->updateRows(static_cast<int>(Vch003::DatabaseTable::CurrentMeas), measurements);   // to DB
        }
        else
            return nextHandle(data);
        return true;
    }


    /*
    0, TDEV, 2084.23, -, -, -, -
    0, MTIE, 1, 15636, -, -, -
    0, df_F, 0, 0, 31248, 31248, -
    4, TDEV, 30.9322, -, -, -, -
    4, MTIE, 1635, 17515, -, -, -
    4, df_F, 41000, 41700, 23137, 23137, -
    9, TDEV, 0.0193604, -, -, -, -
    9, MTIE, 1, 11, -, -, -
    9, df_F, 0, 0, 5, 5, -
    13, TDEV, 4254.96, -, -, -, -
    13, MTIE, 1, 31229, -, -, -
    13, df_F, 0, -100, 18, 18, -
    18, TDEV, 0, -, -, -, -
    18, MTIE, 0, 0, -, -, -
    18, df_F, 0, 0, 31243, 31243, -
    19, TDEV, 0.135937, -, -, -, -
    19, MTIE, 0, 1, -, -, -
    19, df_F, 0, 0, 2, 2, -
    */


    /*
get mlgs 3 30.07.2021 09:20:00 - 30.07.2021 09:25:00
30.07.2021 10:49:16, VCH-003, Test-2021
3, 0, 1627636800, 0.377278, 0, 0, 0
3, 1, 1627636800, 1, 6, 0, 0
3, 2, 1627636800, 28182, 6, 0, 0
3, 0, 1627636860, 0.377278, 0, 0, 0
3, 1, 1627636860, 1, 6, 0, 0
3, 2, 1627636860, 28182, 6, 0, 0
3, 0, 1627636920, 0.377278, 0, 0, 0
3, 1, 1627636920, 1, 6, 0, 0
3, 2, 1627636920, 28182, 6, 0, 0
3, 0, 1627636980, 0.377278, 0, 0, 0
3, 1, 1627636980, 1, 6, 0, 0
3, 2, 1627636980, 28182, 6, 0, 0
3, 0, 1627637040, 0.377278, 0, 0, 0
3, 1, 1627637040, 1, 6, 0, 0
3, 2, 1627637040, 28182, 6, 0, 0
3, 0, 1627637100, 0.377278, 0, 0, 0
3, 1, 1627637100, 1, 6, 0, 0
3, 2, 1627637100, 28182, 6, 0, 0
*/


    /*
get meas
30.07.2021 10:47:51, VCH-003, Test-2021
0, TDEV, 0.606746, 0, 0, 0
0, MTIE, 1, 7, 0, 0
0, df_F, 22943, 7, 0, 0
3, TDEV, 0.377278, 0, 0, 0
3, MTIE, 1, 6, 0, 0
3, df_F, 28182, 6, 0, 0
17, TDEV, 0.149254, 0, 0, 0
17, MTIE, 1, 2, 0, 0
17, df_F, 13864, 2, 0, 0
19, TDEV, 0.0151173, 0, 0, 0
19, MTIE, 37, 380, 0, 0
19, df_F, 1927, 380, 0, 0
*/




}
