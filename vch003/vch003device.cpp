#include "vch003device.h"
#include "configuration/vch003options.h"
#include "configuration/vch003cardcreators.h"
#include "device/configuration/lanconnection.h"
#include "monitorring/responsehandlers/allresponsehandlers.h"
#include "backup/backup.h"
#include "global.h"
#include <QDebug>

#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "database/vch003tables.h"
#include "csettings.h"

namespace ssu {

    static QMap <int, SlotConfiguration> vch003PanelConfiguration = {
        { 0,    SlotConfiguration(SSU::Input, 0) },
        { 1,    SlotConfiguration(SSU::Input, 0) },
        { 2,    SlotConfiguration(SSU::Input, 1) },   // { 2,  SlotConfiguration({ SSU::Input, SSU::Gnss }, 1) },
        { 3,    SlotConfiguration(SSU::Input, 1) },   // { 3,  SlotConfiguration({ SSU::Input, SSU::Gnss }, 1) },
        { 4,    SlotConfiguration(SSU::Gnss, 0) },
        { 5,    SlotConfiguration(SSU::Gnss, 0) },
        { 6,    SlotConfiguration(SSU::Generator, 0) },
        { 8,    SlotConfiguration(SSU::Generator, 0) },
        { 10,   SlotConfiguration(SSU::Output, 0) },
        { 11,   SlotConfiguration(SSU::Output, 0) },
        { 12,   SlotConfiguration(SSU::Distribution, 0) },
        { 13,   SlotConfiguration(SSU::Distribution, 0) },
        { 14,   SlotConfiguration(SSU::Distribution, 1) },
        { 15,   SlotConfiguration(SSU::Distribution, 1) },
        { 16,   SlotConfiguration(SSU::Distribution, 2) },
        { 17,   SlotConfiguration(SSU::Distribution, 2) },
        { 18,   SlotConfiguration(SSU::Monitoring, 0) }
    };

    Vch003Device::Vch003Device(ConnectionSettings *connectionSettings, QObject *parent) : Device(SSU::DeviceType::Vch003,
                                                                                                 connectionSettings,
                                                                                                 new Vch003DeviceOptions,
                                                                                                 vch003PanelConfiguration,
                                                                                                 parent),
        measLogLoading_(false),
        eventLogLoading_(false),
        userActivityLogLoading_(false),
        fileLoading_(false)
    {
        // Список возможных таблиц в БД
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::EventLog),           new Vch003EventLogTable);
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::ActiveEvents),       new Vch003ActiveEventsTable);
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::HiddenEvents),       new Vch003HiddenEventsTable);
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::UserActivityLog),    new Vch003UserActivityLogTable);
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::MeasLog),            new Vch003MeasurementLogTable);
        databaseExecutor_->insertTable(static_cast<int>(Vch003::DatabaseTable::CurrentMeas),        new Vch003CurrentMeasurementTable);

        // Список возможных плат
        panel_->cardFactory()->insertCreator(SSU::CardType::Input,          new InputCardCreator);
        panel_->cardFactory()->insertCreator(SSU::CardType::Output,         new OutputCardCreator);
        panel_->cardFactory()->insertCreator(SSU::CardType::Distribution,   new DistributionCardCreator);
        panel_->cardFactory()->insertCreator(SSU::CardType::Generator,      new GeneratorCardCreator);
        panel_->cardFactory()->insertCreator(SSU::CardType::Monitoring,     new MonitoringCardCreator);
        panel_->cardFactory()->insertCreator(SSU::CardType::Gnss,           new GnssCardCreator);


        //@N Название каналов в платах и ВЗГ..

        // 8 для входных сигналов и 2 для ГНСС
        channelDescription_->setSsuInputChannels({
                                                     { tr("Input") + " 1", "" },
                                                     { tr("Input") + " 2", "" },
                                                     { tr("Input") + " 3", "" },
                                                     { tr("Input") + " 4", "" },
                                                     { tr("Input") + " 5", "" },
                                                     { tr("Input") + " 6", "" },
                                                     { tr("Input") + " 7", "" },
                                                     { tr("Input") + " 8", "" },
                                                     { tr("GNSS/PTP/NTP"), "" },
                                                     { tr("GNSS/PTP/NTP"), "" }
                                                 });

        //@N del tag
        //        for(int i = 0; i < channelDescription_->numberOfSsuInputChannels(); ++i) {
        //            channelDescription_->setInputChannelTag(i, "Tag " + QString::number(i));
        //        }


        ChannelDescription::CardChannels ouputChannels;
        for(int i = 0; i < Vch003DeviceOptions::kNumberOfOutputCardOutChannels; ++i) {
            ouputChannels.append(ChannelDescription::CardChannel(tr("Output"), QString::number(i + 1)));
        }

        channelDescription_->setCardOutputChannels({
                                                       { SSU::CardType::Output,       ouputChannels },
                                                       { SSU::CardType::Distribution, ouputChannels }
                                                   });

        channelDescription_->setCardInputChannels({
                                                      { SSU::CardType::Input, {
                                                            { tr("Input"), "1" },
                                                            { tr("Input"), "2" },
                                                            { tr("Input"), "3" },
                                                            { tr("Input"), "4" }
                                                        }
                                                      },
                                                      { SSU::CardType::Gnss, {
                                                            { tr("Antenna") },
                                                            { tr("1PPS") },
                                                            { tr("PTP") }
                                                        }
                                                      },
                                                      { SSU::CardType::Generator, {
                                                            { tr("Input"), "1", 0 },
                                                            { tr("Input"), "2", 0 },
                                                            { tr("Input"), "3", 0 },
                                                            { tr("Input"), "4", 0 },
                                                            { tr("Input"), "1", 1 },
                                                            { tr("Input"), "2", 1 },
                                                            { tr("Input"), "3", 1 },
                                                            { tr("Input"), "4", 1 },
                                                            { tr("Input"), "1", 2 },
                                                            { tr("Input"), "2", 2 },
                                                            { tr("Input"), "3", 2 },
                                                            { tr("Input"), "4", 2 },
                                                            { tr("Input"), "1", 3 },
                                                            { tr("Input"), "2", 3 },
                                                            { tr("Input"), "3", 3 },
                                                            { tr("Input"), "4", 3 },
                                                            { tr("GNSS"),  "", 4 },
                                                            { tr("GNSS"),  "", 5 }
                                                        }
                                                      },
                                                      { SSU::CardType::Output, {
                                                            { tr("Input"),     "1", 0 },
                                                            { tr("Input"),     "2", 0 },
                                                            { tr("Input"),     "3", 0 },
                                                            { tr("Input"),     "4", 0 },
                                                            { tr("Input"),     "1", 1 },
                                                            { tr("Input"),     "2", 1 },
                                                            { tr("Input"),     "3", 1 },
                                                            { tr("Input"),     "4", 1 },
                                                            { tr("Input"),     "1", 2 },
                                                            { tr("Input"),     "2", 2 },
                                                            { tr("Input"),     "3", 2 },
                                                            { tr("Input"),     "4", 2 },
                                                            { tr("Input"),     "1", 3 },
                                                            { tr("Input"),     "2", 3 },
                                                            { tr("Input"),     "3", 3 },
                                                            { tr("Input"),     "4", 3 },
                                                            { tr("GNSS"),      "",  4 },
                                                            { tr("GNSS"),      "",  5 },
                                                            { tr("Generator"), "",  6 },
                                                            { tr("Generator"), "",  8 }
                                                        }
                                                      }
                                                  });


        // Создадим цепочку обязаностей для разбора сообщений

        auto grettingsMessageHandler =          new GrettingsMessageHandler;

        auto trapResponse =                     new TrapParseHandler(this);
        auto cardParamResponse =                new GetParametersReponseHandler(this);
        auto inveResponse =                     new InventoryResponseHandler(this);

        auto getSatelliteListReponseHandler = new GetSatelliteListReponseHandler(this);
        auto getGnssPortSettingsReponseHandler = new GetGnssPortSettingsReponseHandler(this);
        auto getGnssPortStatisticsReponseHandler = new GetGnssPortStatisticsReponseHandler(this);
        auto getGnssNetworkConfigurationReponseHandler = new GetGnssNetworkConfigurationReponseHandler(this);


        auto userListReponseHandler =           new UserListReponseHandler(this);
        auto addUserReponseHandler =            new AddUserReponseHandler(this);
        auto deleteUserReponseHandler =         new DeleteUserReponseHandler(this);
        auto changeUserPasswordReponseHandler = new ChangeUserPasswordReponseHandler(this);
        auto blockUserReponseHandler =          new BlockUserReponseHandler(this);
        auto getPanelConfigReponseHandler =     new GetPanelConfigReponseHandler(this);
        auto setAllThresholdsReponseHandler =   new SetAllThresholdsReponseHandler(this);
        auto resetThresholdsReponseHandler =    new ResetThresholdsReponseHandler(this);
        auto getThresholdReponseHandler =       new GetThresholdReponseHandler(this);

        auto measResetReponseHandler =          new MeasResetReponseHandler(this);
        auto userInfoResponse =                 new GetCurrentUserInfoResponseHandler(this);
        auto eventDescriptionReponse =          new GetEventDescriptionReponseHandler(this);

        auto getActiveEventsReponseHandler =    new GetActiveEventsReponseHandler(this);
        auto getEventLogReponseHandler =        new GetEventLogReponseHandler(this);
        auto getUserLogReponseHandler =         new GetUserLogReponseHandler(this);
        auto getReclistReponseHandler =         new GetReclistReponseHandler(this);
        auto getMeasLogReponseHandler =         new GetMeasLogReponseHandler(this);
        auto getCurrentMeasReponseHandler =     new GetCurrentMeasReponseHandler(this);


        responseHandler_ = grettingsMessageHandler;
        responseHandler_->setNext(trapResponse);
        trapResponse->setNext(cardParamResponse);
        cardParamResponse->setNext(inveResponse);
        inveResponse->setNext(getActiveEventsReponseHandler);

        getActiveEventsReponseHandler->setNext(getCurrentMeasReponseHandler);
        getCurrentMeasReponseHandler->setNext(getEventLogReponseHandler);

        getEventLogReponseHandler->setNext(getUserLogReponseHandler);
        getUserLogReponseHandler->setNext(getMeasLogReponseHandler);
        getMeasLogReponseHandler->setNext(getReclistReponseHandler);
        getReclistReponseHandler->setNext(userListReponseHandler);



        userListReponseHandler->setNext(getSatelliteListReponseHandler);
        getSatelliteListReponseHandler->setNext(getGnssPortSettingsReponseHandler);
        getGnssPortSettingsReponseHandler->setNext(getGnssPortStatisticsReponseHandler);
        getGnssPortStatisticsReponseHandler->setNext(getGnssNetworkConfigurationReponseHandler);
        getGnssNetworkConfigurationReponseHandler->setNext(addUserReponseHandler);



        addUserReponseHandler->setNext(deleteUserReponseHandler);
        deleteUserReponseHandler->setNext(changeUserPasswordReponseHandler);
        changeUserPasswordReponseHandler->setNext(blockUserReponseHandler);
        blockUserReponseHandler->setNext(getPanelConfigReponseHandler);
        getPanelConfigReponseHandler->setNext(getThresholdReponseHandler);
        getThresholdReponseHandler->setNext(eventDescriptionReponse);

        //set thres


        eventDescriptionReponse->setNext(setAllThresholdsReponseHandler);
        setAllThresholdsReponseHandler->setNext(resetThresholdsReponseHandler);
        resetThresholdsReponseHandler->setNext(userInfoResponse);
        userInfoResponse->setNext(measResetReponseHandler);



        for(int i = 0; i < channelDescription_->numberOfSsuInputChannels(); ++i) {
            MaskData mask;
            mask.name = channelDescription_->ssuInputChannelName(i);
            mask.data.set(static_cast<int>(SSU::MeasDataType::Mtie), { 751e-9, 1000e-9, 2500e-9, 4500e-9 });     // { 750e-9, 1000e-9, 2500e-9, 4500e-9, 5000e-9 };
            mask.data.set(static_cast<int>(SSU::MeasDataType::Tdev), { 34e-9, 34e-9, 130e-9, 170e-9 });          // { 34e-9, 34e-9, 130e-9, 170e-9, 540e-9 };
            thresholdList_->append(mask);
        }


        // События
        eventDescriptionList_->append({
                                          // Input card
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::CardIn),                      tr("Card In"),                              SSU::EventStatus::NotAlarm },        //@N ? а нужно ли это..
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::CartOut),                     tr("Card Out"),                             SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::NoActiveCard),                tr("No active card or both card active"),   SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::WrongSlot),                   tr("Wrong slot"),                           SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),        tr("Card not fully inserted"),              SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::DisqualificationByStatus),    tr("Disqualification by status"),           SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::LossOfSignal),                tr("Loss of signal"),                       SSU::EventStatus::Major },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::LossOfFrame),                 tr("Loss of frame"),                        SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::LossOfMultiframe),            tr("Loss of multiframe"),                   SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::SignalAppear),                tr("Appearance of a signal at the input"),  SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::SignalDelay),                 tr("Signal recovery"),                      SSU::EventStatus::Warning },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::InpSSMChanged),               tr("Input SSM changed"),                    SSU::EventStatus::NotAlarm },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::InpTypChanged),               tr("Input type changed"),                   SSU::EventStatus::NotAlarm },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::ErrTransmEnd),                tr("Failure at the transmitting end"),      SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::ErrCRC),                      tr("Missing (Mismatch) CRC"),               SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::ErrHDB3),                     tr("HDB3 errors"),                          SSU::EventStatus::Minor },
                                          { SSU::Input,     static_cast<int>(Vch003::DeviceEvent::ErrFreqCapt),                 tr("No frequency capture"),                 SSU::EventStatus::Minor },


                                          // Output card
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::CardIn),                      tr("Card In"),                          SSU::EventStatus::NotAlarm },        //@N ? а нужно ли это..
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::CartOut),                     tr("Card Out"),                         SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::NoActiveCard),                tr("No active card or both card active"),   SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::WrongSlot),                   tr("Wrong slot"),                           SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),        tr("Card not fully inserted"),              SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::ChangedToPassive),            tr("Changed to passive"),               SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::ChangedToActive),             tr("Changed to active"),                SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::ActiveInputChanged),          tr("Active input changed"),             SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::BackupInputChanged),          tr("Backup input changed"),             SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::PriorityTableError),          tr("Priority table error"),             SSU::EventStatus::Major },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::ActiveGeneratorFailure),      tr("Active generator failure"),         SSU::EventStatus::Major },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::BackupGeneratorFailure),      tr("Backup generator failure"),         SSU::EventStatus::Major },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::DisqualificationInternalUSS), tr("Disqualification internal USS"),    SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::BackupChannelNoSync),         tr("Backup channel no sync"),           SSU::EventStatus::Major },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::BothChanNoSync),              tr("No channel sync"),                  SSU::EventStatus::Minor },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::Sync500Fault),                tr("Sync 500 Hz Failure"),              SSU::EventStatus::Minor },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::Bypass),                      tr("Bypass mode"),                      SSU::EventStatus::Major },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::OutSSMmodeChanged),           tr("Output SSM mode changed"),          SSU::EventStatus::NotAlarm },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::OutTypChanged),               tr("Output type changed"),              SSU::EventStatus::NotAlarm },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::OutSSMChanged),               tr("Output SSM changed"),               SSU::EventStatus::NotAlarm },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::NoMeasMsgs),                  tr("No messages with measurements"),    SSU::EventStatus::Warning },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::ExecCmdChanSwitch),           tr("Active channel change in progress"),SSU::EventStatus::NotAlarm },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::DisqualificationByMTIE),      tr("Disqualification by MTIE"),         SSU::EventStatus::Minor },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::DisqualificationByTDEV),      tr("Disqualification by TDEV"),         SSU::EventStatus::Minor },
                                          { SSU::Output,    static_cast<int>(Vch003::DeviceEvent::DisqualificationByDff),       tr("Disqualification by dF/F"),         SSU::EventStatus::Minor },


                                          // Distribution card
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::CardIn),                  tr("Card In"),                          SSU::EventStatus::NotAlarm },        //@N ? а нужно ли это..
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::CartOut),                 tr("Card Out"),                         SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::NoActiveCard),            tr("No active card or both card active"),   SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::WrongSlot),               tr("Wrong slot"),                           SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),    tr("Card not fully inserted"),              SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::ChangedToPassive),        tr("Changed to passive"),               SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::ChangedToActive),         tr("Changed to active"),                SSU::EventStatus::Warning },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::SignalGenerationError),   tr("Signal generation error"),          SSU::EventStatus::Minor },
                                          { SSU::Distribution,  static_cast<int>(Vch003::DeviceEvent::OutTypChanged),           tr("Output type changed"),              SSU::EventStatus::NotAlarm },


                                          // Generator card
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::CardIn),                      tr("Card In"),                          SSU::EventStatus::NotAlarm },        //@N ? а нужно ли это..
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::CartOut),                     tr("Card Out"),                         SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::NoActiveCard),                tr("No active card or both card active"),   SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::WrongSlot),                   tr("Wrong slot"),                           SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),        tr("Card not fully inserted"),              SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::ActiveInputChanged),          tr("Active input changed"),             SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::PriorityTableError),          tr("Priority table error"),             SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::Warmup),                      tr("Warmup"),                           SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::Freerun),                     tr("Freerun"),                          SSU::EventStatus::Minor },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::Tracking),                    tr("Tracking"),                         SSU::EventStatus::NotAlarm },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::Holdover),                    tr("Holdover"),                         SSU::EventStatus::Minor },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::Adjustment),                  tr("Adjustment"),                       SSU::EventStatus::Warning },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::GeneratorERROR),              tr("ERROR"),                            SSU::EventStatus::Critical },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::ActiveSignalFailure),         tr("Active signal failure"),            SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::GeneratorFailure),            tr("Generator failure"),                SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::DDSFailure),                  tr("DDS failure"),                      SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::MeasurmentSystemFailure),     tr("Measurement system failure"),       SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::ControlLoopFailure),          tr("Control loop failure"),             SSU::EventStatus::Major },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::GModeChanged),                tr("Changing generator mode"),          SSU::EventStatus::NotAlarm },
                                          { SSU::Generator,     static_cast<int>(Vch003::DeviceEvent::GenManInpChanged),            tr("Generator manual input changed"),   SSU::EventStatus::NotAlarm },

                                          // Gnss
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::CardIn),                      tr("Card In"),                          SSU::EventStatus::NotAlarm },        //@N ? а нужно ли это..
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::CartOut),                     tr("Card Out"),                         SSU::EventStatus::Major },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::NoActiveCard),                tr("No active card or both card active"),   SSU::EventStatus::Warning },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::WrongSlot),                   tr("Wrong slot"),                           SSU::EventStatus::Warning },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),        tr("Card not fully inserted"),              SSU::EventStatus::Warning },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::DisqInput),                   tr("Input disqualification"),           SSU::EventStatus::Major },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::GnssModeChanged),             tr("Change of operating mode"),         SSU::EventStatus::NotAlarm },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::GnssNoHold),                  tr("No capture"),                       SSU::EventStatus::Major },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::GnssAdjust),                  tr("Adjustment"),                       SSU::EventStatus::NotAlarm },
                                          { SSU::Gnss,          static_cast<int>(Vch003::DeviceEvent::GnssHoldover),                tr("Holdover"),                         SSU::EventStatus::Major },


                                          // Monitoring card
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::CardIn),                      tr("Card In"),                          SSU::EventStatus::NotAlarm },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::WrongSlot),                   tr("Wrong slot"),                       SSU::EventStatus::Warning },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::CardNotFullyInserted),        tr("Card not fully inserted"),          SSU::EventStatus::Warning },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::PowerAFailure),               tr("Power A failure"),                  SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::PowerBFailure),               tr("Power B failure"),                  SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::ConnectionLimitExceeded),     tr("Connection limit exceeded"),        SSU::EventStatus::Warning },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::EventLogOverflow),            tr("Event log overflow"),               SSU::EventStatus::NotAlarm },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::UserActivityLogOverflow),     tr("User activity log overflow"),       SSU::EventStatus::NotAlarm },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::MeasurementLogOverflow),      tr("Measurement log overflow"),         SSU::EventStatus::NotAlarm },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::FuseAfail),                   tr("Fuse failure in power card A"),     SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::FuseBfail),                   tr("Fuse failure in power card B"),     SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::PowerAout),                   tr("Power cart A OUT"),                 SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::PowerBout),                   tr("Power cart B OUT"),                 SSU::EventStatus::Minor },
                                          { SSU::Monitoring,    static_cast<int>(Vch003::DeviceEvent::ConfigChanged),               tr("Cards configuration changed"),      SSU::EventStatus::NotAlarm }
                                      });

        activeEvents_ = new ActiveEventModel(this);
        activeEvents_->setDevice(this);

        monitoring_ = new Vch003Monitoring;
        monitoring_->setConnectionSettings(connectionSettings_);
        QObject::connect(monitoring_, &Vch003Monitoring::readyRead, this, &Vch003Device::readyRead);

        if(SSU::kImitationWork) {
            deviceConnected();
        }
    }

    Vch003Device::~Vch003Device()
    {
        qInfo() << "~Vch003Device() start";

        delete monitoring_;
        delete responseHandler_;

        qInfo() << "~Vch003Device() end";
    }

    void Vch003Device::connectToDevice()
    {
        monitoring_->connectToDevice();
    }

    void Vch003Device::authorization()
    {
        monitoring_->authorization();
    }

    void Vch003Device::disconnectFromDevice()
    {
        monitoring_->disconnectFromDevice();
    }

    bool Vch003Device::send(Request *request) const
    {
        return monitoring_->send(request);
    }

    bool Vch003Device::sendList(QList<Request *> list) const
    {
        for(int i = 0; i < list.size(); ++i) {
            send(list.at(i));
        }
        return true;
    }

    void Vch003Device::deviceConnected()
    {
        qInfo() << "Vch003Device::deviceConnected";

        Device::deviceConnected();

        send(new vch003request::GetCurrentUserInfo);
        send(new vch003request::GetPanelConfiguration);

        auto getInventoryRequest = new vch003request::GetInventory;
        getInventoryRequest->setDelay(3000);
        monitoring_->updatePollQueue({ getInventoryRequest });  // Будем опрашивать список инвентаризации


        //@N Вот это переделать!! Нужно будет завязываться на серийном номере корзины. connectionSettings_->name() на другое переделать..
        database_->open(SSU::programDatabaseFile(connectionSettings_->name()), VCH_003_DATABASE_VERSION, connectionSettings_->name());
        activeEvents_->update();

        updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::UserActivityLog));
        updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::EventLog));
        updateDownloadTime(static_cast<int>(Vch003::DatabaseTable::MeasLog));
    }

    void Vch003Device::deviceDisconnected()
    {
        qInfo() << "Vch003Device::deviceDisconnected";

        setMeasLogLoading(false);
        setEventLogLoading(false);
        setUserActivityLogLoading(false);
        setFileLoading(false);

        activeEvents_->clear();
        lastDownloadTimeHash_.clear();

        // clear all..
        Device::deviceDisconnected();
    }

    void Vch003Device::readyRead(const QString &data)
    {
        responseHandler_->handle(data);
    }
    
    bool Vch003Device::fileLoading() const
    {
        return fileLoading_;
    }
    
    void Vch003Device::setFileLoading(bool fileLoading)
    {
        if(fileLoading_ != fileLoading) {
            fileLoading_ = fileLoading;
            emit deviceDataLoadingChanged();
        }
    }

    void Vch003Device::updateDownloadTime(int table)
    {
        // Обновим метку времени
        bool isok;
        auto lastSec = databaseExecutor_->lastValue(table, &isok);

        if(!isok || lastSec.isNull())
            return;

        auto it = lastDownloadTimeHash_.find(table);
        if(it == lastDownloadTimeHash_.end()) {
            lastDownloadTimeHash_.insert(table, lastSec.toLongLong());
        }
        else {
            it.value() = lastSec.toLongLong();
        }
    }

    qint64 Vch003Device::lastDownloadTime(int table) const
    {
        auto it = lastDownloadTimeHash_.constFind(table);
        return it == lastDownloadTimeHash_.constEnd() ? 0
                                                      : it.value();
    }

    bool Vch003Device::userActivityLogLoading() const
    {
        return userActivityLogLoading_;
    }

    void Vch003Device::setUserActivityLogLoading(bool userActivityLogLoading)
    {
        if(userActivityLogLoading_ != userActivityLogLoading) {
            userActivityLogLoading_ = userActivityLogLoading;
            emit deviceDataLoadingChanged();
        }
    }

    bool Vch003Device::eventLogLoading() const
    {
        return eventLogLoading_;
    }

    void Vch003Device::setEventLogLoading(bool eventLogLoading)
    {
        if(eventLogLoading_ != eventLogLoading) {
            eventLogLoading_ = eventLogLoading;
            emit deviceDataLoadingChanged();
        }
    }

    bool Vch003Device::measLogLoading() const
    {
        return measLogLoading_;
    }

    void Vch003Device::setMeasLogLoading(bool measLogLoading)
    {
        if(measLogLoading_ != measLogLoading) {
            measLogLoading_ = measLogLoading;
            emit deviceDataLoadingChanged();
        }
    }

    ActiveEventModel *Vch003Device::activeEvents() const
    {
        return activeEvents_;
    }

    Vch003Monitoring *Vch003Device::monitoring() const
    {
        return monitoring_;
    }

    QString Vch003Device::name() const
    {
        return tr("Vch-003");
    }

    bool Vch003Device::uploadBackup(const QString &backupDir)
    {
        Backup backup(backupDir);
        if(!backup.exists())
            return false;
        return send(new vch003request::UploadConfig(backupDir,
                                                    this));
    }

    bool Vch003Device::uploadTypeFirmware(SSU::CardType type, const QString &pathToFile)
    {
        if(type == SSU::CardType::InvalidType) return false;
        QFileInfo f(pathToFile);
        if(f.exists())
            return false;
        return send(new vch003request::UploadFirmware(f.absolutePath(), f.fileName(), type, this));
    }

    bool Vch003Device::uploadCardFirmware(const QString &slot, const QString &pathToFile)
    {
        if(slot.isEmpty()) return false;
        QFileInfo f(pathToFile);
        if(f.exists())
            return false;
        return send(new vch003request::UploadFirmware(f.absolutePath(), f.fileName(), slot, this));
    }

    bool Vch003Device::downloadBackup(const QString &backupDir)
    {
        return send(new vch003request::DownloadConfig(backupDir + "/" + QDateTime::currentDateTime().toString("dd.MM.yyyy HH.mm.ss"),
                                                      this));
    }

    bool Vch003Device::updateInventory() const
    {
        return send(new vch003request::GetInventory);
    }

    bool Vch003Device::updateCardEventDescription(SSU::CardType type) const
    {
        return send(new vch003request::GetEventDescription(static_cast<SSU::CardType>(type)));
    }

    bool Vch003Device::updateEventDescription() const
    {
        for(const int &type : panel_->cardFactory()->types()) {
            if(!send(new vch003request::GetEventDescription(static_cast<SSU::CardType>(type))))
                return false;
        }
        return true;
    }

    bool Vch003Device::updateUserList() const
    {
        if(options_->userRights()->deviceControl(connectionSettings_->level()))
            return send(new vch003request::GetUserList);
        return false;
    }

    bool Vch003Device::addUser(const QString &username, const QString &password, SSU::UserLevel level) const
    {
        return send(new vch003request::AddUser(username, password, level));
    }

    bool Vch003Device::deleteUser(const QString &username) const
    {
        return send(new vch003request::DeleteUser(username));
    }

    bool Vch003Device::changeUserPassword(const QString &username, const QString &password) const
    {
        return send(new vch003request::SetUserPassword(username, password));
    }

    bool Vch003Device::blockUser(const QString &username, const QDateTime &untilDate) const
    {
        return send(new vch003request::BlockUser(username, untilDate));
    }

    bool Vch003Device::unblockUser(const QString &username) const
    {
        return send(new vch003request::UnblockUser(username));
    }

    bool Vch003Device::resetCard(const QString &slot) const
    {
        return send(new vch003request::ResetCard(slot));
    }

    bool Vch003Device::resetToDefaultCard(const QString &slot) const
    {
        return send(new vch003request::ResetToDefaultCard(slot));
    }

    bool Vch003Device::resetUserActivityLog() const
    {
        return send(new vch003request::ResetUserLog);
    }

    bool Vch003Device::resetEventLog() const
    {
        return send(new vch003request::ResetEventLog);
    }

    bool Vch003Device::resetMeasLog() const
    {
        return send(new vch003request::ResetMeasLog);
    }

    bool Vch003Device::resetMeasurements(int channel) const
    {
        return send(new vch003request::ResetMeasurements(channel));
    }

    void Vch003Device::clearMeasTable()
    {
        databaseExecutor_->drop(static_cast<int>(Vch003::DatabaseTable::MeasLog));
    }

    void Vch003Device::clearUserActivityLogTable()
    {
        databaseExecutor_->drop(static_cast<int>(Vch003::DatabaseTable::UserActivityLog));
    }

    void Vch003Device::clearEventsLogTable()
    {
        databaseExecutor_->drop(static_cast<int>(Vch003::DatabaseTable::EventLog));
    }

    bool Vch003Device::changeThreshold(int channel, const DataContainer &data)
    {
        // пройдемся. Посмотрим есть ли изменения..
        auto mask = thresholdList_->mask(channel);
        auto list = mask.data.compare(data);

        if(list.isEmpty())
            return false;

        int count = 0;
        for(const auto &m : list) {
            if(m.second >= options_->kThresholdIntervals.size())
                continue;

            auto value = data.value(m.first, m.second);
            qInfo() << m.first << m.second << value << qIsNaN(value);
            if(qIsNaN(value)) {
                value = 0;
            }
            else value /= vch003request::getThresholdCoef(static_cast<SSU::MeasDataType>(m.first));

            if(value > INT_MAX) continue;
            value = qAbs(value);

            // Тк сейчас пороговые значения дублируются, то нужно отправить в два канала..
            for(const int &ch : Vch003DeviceOptions::inputLinkedChannels.at(channel)) {
                if(send(new vch003request::SetThreshold(ch, static_cast<SSU::MeasDataType>(m.first), m.second, qRound(value)))) {
                    ++count;
                }
            }
        }

        if(count > 0) {
            // Обновим
            auto cmd = new vch003request::GetThresholds;
            cmd->setDelay(SSU::kDelayBeforeCheckRequest);
            send(cmd);

            if(SSU::kChangedParamImmediately) {
                // Тк сейчас пороговые значения дублируются, то нужно отправить в два канала..
                thresholdList_->setMaskData(channel, data);
            }
        }
        return true;
    }

    bool Vch003Device::copyThresholdsToAllChannels(int channel)
    {
        // Тк сейчас пороговые значения дублируются, нужно найти нужный нам канал
        if(channel < 0 || channel >= Vch003DeviceOptions::inputLinkedChannels.size())
            return false;
        auto inputs = Vch003DeviceOptions::inputLinkedChannels.at(channel);
        if(inputs.isEmpty())
            return false;
        auto cmd = new vch003request::SetAllThreshols(inputs.first());
        cmd->setDelay(SSU::kDelayForSendingRequest);
        return send(cmd);
    }

    bool Vch003Device::applyThresholdsToAllChannels(const DataContainer &data)
    {
        if(!changeThreshold(0, data))
            return false;
        return send(new vch003request::SetAllThreshols(0));
    }

    bool Vch003Device::resetThresholds() const
    {
        return send(new vch003request::ResetThresholds);
    }

    bool Vch003Device::setTag(int channel, const QString &tag)
    {
        //@N add command
        return Device::setTag(channel, tag);
    }

    bool Vch003Device::changeEventStatus(SSU::CardType type, int eventId, SSU::EventStatus status)
    {
        eventDescriptionList_->updateStatus(type, eventId, status);
        return send(new vch003request::SetCardEventStatus(type, eventId, status));
    }

    bool Vch003Device::updateActiveEvents() const
    {
        return send(new vch003request::GetActiveEvents);
    }

    bool Vch003Device::downloadUserLog()
    {
        return send(new vch003request::GetUserLog(this));
    }

    bool Vch003Device::downloadEventLog()
    {
        return send(new vch003request::GetEventLog(this));
    }

    bool Vch003Device::downloadReclist()
    {
        return send(new vch003request::GetReclist(this));
    }

    bool Vch003Device::downloadUserLog(const QDateTime &date1, const QDateTime &date2)
    {
        if(!date1.isValid())
            return false;
        // первая дата не может быть пустой, а вторая может
        return date2.isValid() ? send(new vch003request::GetUserLog(date1, date2, this))
                               : send(new vch003request::GetUserLog(date1, QDateTime(), this));
    }

    bool Vch003Device::downloadEventLog(const QDateTime &date1, const QDateTime &date2)
    {
        if(!date1.isValid())
            return false;
        // первая дата не может быть пустой, а вторая может
        return date2.isValid() ? send(new vch003request::GetEventLog(date1, date2, this))
                               : send(new vch003request::GetEventLog(date1, QDateTime(), this));
    }

    bool Vch003Device::downloadReclist(const QDateTime &date1, const QDateTime &date2)
    {
        if(!date1.isValid())
            return false;
        // первая дата не может быть пустой, а вторая может
        return date2.isValid() ? send(new vch003request::GetReclist(date1, date2, this))
                               : send(new vch003request::GetReclist(date1, QDateTime(), this));
    }

    bool Vch003Device::updateCurrentMeas()
    {
        return send(new vch003request::GetCurrentMeas);
    }

    bool Vch003Device::updatePanelConfiguration() const
    {
        return send(new vch003request::GetPanelConfiguration);
    }

    bool Vch003Device::setPanelConfiguration(const QList<bool> &availableCards, const QList<int> &types)
    {
        // Запишем в json
        QJsonArray cardsArray;

        for(int i = 0; i < panel_->panelConfiguration()->size(); ++i) {
            if(i >= availableCards.size() || i >= types.size())
                continue;
            if(availableCards.at(i)) {
                SSU::CardType type = static_cast<SSU::CardType>(types.at(i));
                if(type == SSU::CardType::InvalidType || type == SSU::CardType::Stub)
                    continue;

                QJsonObject obj;
                obj.insert("cardType", vch003request::cardTypeToString(type));
                obj.insert("slot", SSU::addressToString(panel_->panelConfiguration()->at(i)->address()));
                cardsArray.append(obj);
            }
        }

        QJsonDocument doc; doc.setArray(cardsArray);

        QDir dir(TEMP_DIR_PATH);
        if(!dir.exists()) dir.mkpath(dir.absolutePath());

        QFile file(TEMP_DIR_PATH + "/" + DEVICE_PANEL_CONFIG_FILE_NAME);
        if(file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        }

        // и отправим на устройство
        return send(new vch003request::SetPanelConfiguration(TEMP_DIR_PATH, DEVICE_PANEL_CONFIG_FILE_NAME));
    }

    bool Vch003Device::resetDevice() const
    {
        return send(new vch003request::ResetDevice);
    }

    bool Vch003Device::updateThresholds() const
    {
        return send(new vch003request::GetThresholds);
    }

}


