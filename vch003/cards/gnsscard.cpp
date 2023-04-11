#include "gnsscard.h"
#include "ssu.h"
#include "vch003/configuration/vch003options.h"

#include <QRegularExpression>
#include <QTextStream>

#include "vch003/channels/inchannelgnsscard.h"
#include "ssu.h"

#include "csettings.h"

namespace ssu {
    GnssCard::GnssCard(int address, QObject *parent) : Vch003Card(SSU::Gnss, address, parent),
        ban_(false),
        measurementMode_(false),
        delayToRemoveBanByMeasurements_(1),
        pllState_(SSU::PllState::NoCapture),
        pllStateTimer_(0),
        referenceGeneratorAddress_(-1),
        activeChannel_(0),
        manualChannel_(0),
        mode_(SSU::WorkingMode::ManualMode),
        ssm_(SSU::SSM::None)
    {
        leds_ = new LedList({
                                Qt::darkGreen,     // Питание

                                // Входы
                                Qt::darkGreen,     // Antenna
                                Qt::darkGreen,     // 1 PPS
                                Qt::darkGreen,     // PTP

                                // Захват
                                Qt::darkGreen,     // Antenna
                                Qt::darkGreen,     // 1 PPS
                                Qt::darkGreen,     // PTP

                                Qt::red            // Авария
                            },
                            this);


        for(int i = 0; i < Vch003DeviceOptions::kNumberOfGnssCardInChannels; ++i) {
            auto channel = new InChannelGnssCard(this);
            channel->setPriority(i);
            inputChannels_.append(channel);
        }

        satelliteList_ = new SatelliteListModel(this);

        gnss_ = new GnssModule(this);

        port0_ = new EthernetPort(this);
        port1_ = new EthernetPort(this);

        if(SSU::kImitationWork) {
            referenceGeneratorAddress_ = 8;
            pllStateTimer_ = 300;

            satelliteList_->set({
                                    { static_cast<int>(SSU::SatelliteSystem::GPS), 2, 9, 314, 37, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GPS), 4, 30, 310, 36, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GPS), 5, 29, 101, 42, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GPS), 7, 3, 137, 44, false },
                                    { static_cast<int>(SSU::SatelliteSystem::GPS), 8, 43, 41, 38, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GLONASS), 65, 10, 064, 35, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GLONASS), 66, 38, 157, 37, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GLONASS), 67, 76, 210, 11, false },
                                    { static_cast<int>(SSU::SatelliteSystem::GLONASS), 75, 24, 48, 42, true },
                                    { static_cast<int>(SSU::SatelliteSystem::GLONASS), 176, 11, 26, 46, true },
                                });

            port0_->setMacAddress("FF-FF-FF-FF-FF-FF");
            port0_->setSpeed(EthernetPort::Speed::M100);

            port0_->networkConfigurationList().at(2)->setValid(true);
            port0_->networkConfigurationList().at(2)->setIp4("1.2.3.4");
            port0_->networkConfigurationList().at(2)->setMask("5.6.7.8");
            port0_->networkConfigurationList().at(2)->setGateway("9.10.11.12");
            port0_->networkConfigurationList().at(2)->ntp()->setValid(true);
            port0_->networkConfigurationList().at(2)->ptp()->setValid(true);
            port0_->networkConfigurationList().at(2)->ptp()->setPtpMode(Ptp::PtpMode::Slave);

            port0_->networkConfigurationList().at(3)->setValid(true);
            port0_->networkConfigurationList().at(3)->setIp4("1.2.3.4");
            port0_->networkConfigurationList().at(3)->setMask("5.6.7.8");
            port0_->networkConfigurationList().at(3)->setGateway("9.10.11.12");
            port0_->networkConfigurationList().at(3)->ntp()->setValid(true);

            port0_->networkConfigurationList().at(4)->setValid(true);
            port0_->networkConfigurationList().at(4)->setIp4("1.2.3.4");
            port0_->networkConfigurationList().at(4)->setMask("5.6.7.8");
            port0_->networkConfigurationList().at(4)->setGateway("9.10.11.12");
            port0_->networkConfigurationList().at(4)->ptp()->setValid(true);

            port0_->networkConfigurationList().at(8)->setValid(true);
            port0_->networkConfigurationList().at(8)->setIp4("1.2.3.4");
            port0_->networkConfigurationList().at(8)->setMask("5.6.7.8");
            port0_->networkConfigurationList().at(8)->setGateway("9.10.11.12");

            //            port1_->setSpeed(EthernetPort::Speed::M1000);
            //            port1_->networkConfigurationList().at(2)->setValid(true);
            //            port1_->networkConfigurationList().at(2)->setIp4("1.2.3.4");
            //            port1_->networkConfigurationList().at(2)->setMask("5.6.7.8");
            //            port1_->networkConfigurationList().at(2)->setGateway("9.10.11.12");
            //            port1_->networkConfigurationList().at(2)->ntp()->setValid(true);
            //            port1_->networkConfigurationList().at(2)->ptp()->setValid(true);
            //            port1_->networkConfigurationList().at(2)->ptp()->setPtpMode(Ptp::PtpMode::Slave);

            gnss_->setSelectedSystems({ static_cast<int>(SSU::SatelliteSystem::Galileo), static_cast<int>(SSU::SatelliteSystem::BeiDou) });
        }
    }

    void GnssCard::parseParameters(const QString &parameterList)
    {
        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        bool isok;

        while(stream.readLineInto(&line)) {
            if(line.isEmpty())
                continue;

            // Mode
            re.setPattern("^mode, ([^-]+)-?(\\d+)?");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto gmode = vch003request::workingModeFromString(value);
                setMode(gmode);
                if(match.capturedLength() == 2) {
                    int channel = match.capturedRef(2).toInt(&isok);
                    if(isok)
                        setManualChannel(channel);
                }

                continue;
            }

            // State
            re.setPattern("^state, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    setPllState(vch003request::pllStateFromString(value));
                continue;
            }

            // Active chanel number
            re.setPattern("^actC, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    setActiveChannel(value);
                continue;
            }

            re.setPattern("^measMode, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setMeasurementMode(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            //wtBan
            re.setPattern("^wtBan, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                int value = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                setDelayToRemoveBanByMeasurements(value);
                continue;
            }

            re.setPattern("^banFPGA, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setBan(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // Restore Time
            re.setPattern("^wtRT(\\d+), (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                int value = match.capturedRef(2).toInt(&isok);
                if(!isok) continue;
                static_cast<InChannelGnssCard*>(inputChannels_[index - 1])->setWaitToRestoreTime(value);
                continue;
            }

            // Config
            re.setPattern("^power(\\d+), (OFF|ON)");  // (.+)
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelGnssCard*>(inputChannels_[index - 1])->setConfiguration(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // State
            re.setPattern("^power(\\d+), (OFF|ON)");  // (.+)
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelGnssCard*>(inputChannels_[index - 1])->setState(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // Chanels table of priorities
            re.setPattern("^priT, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto priT = value.split(" ");
                for(int i = 0; i < priT.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    int pri = priT.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelGnssCard*>(inputChannels_[i])->setPriority(pri);
                }
                continue;
            }

            // SSM
            re.setPattern("^ssm, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setSsm(vch003request::ssmFromString(value));
                continue;
            }

            re.setPattern("^pdop, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toDouble(&isok);
                if(isok)
                    gnss_->setPdop(value);
                continue;
            }

            re.setPattern("^pdopMask, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toDouble(&isok);
                if(isok)
                    gnss_->setPdopMask(value);
                continue;
            }

            re.setPattern("^dateTime, (\\d{2}.\\d{2}.\\d{2} \\d{2}:\\d{2}:\\d{2}:\\d{3})");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = QDateTime::fromString(match.capturedRef(1).toString(), "dd.MM.yy HH:mm:ss:zzz");
                if(isok && value.isValid()) {
                    value.setTimeSpec(Qt::UTC);
                    gnss_->setCurrentDateTime(value);
                }
                continue;
            }

            re.setPattern("^measDataValid, (\\d)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setNmeasDataValidity(value == 1);
                continue;
            }

            re.setPattern("^antennaCableDelay, (-?\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setCableDelay(value);
                continue;
            }

            re.setPattern("^latitude, (.+) (N|W|S|E)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toDouble(&isok);
                if(isok) {
                    auto h = vch003request::hemisphereFromString(match.capturedRef(2));
                    gnss_->setLatitude({ h, value });
                }
                continue;
            }

            re.setPattern("^longtitude, (.+) (N|W|S|E)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toDouble(&isok);
                if(isok) {
                    auto h = vch003request::hemisphereFromString(match.capturedRef(2));
                    gnss_->setLongitude({ h, value });
                }
                continue;
            }

            re.setPattern("^altitude, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                double value = match.capturedRef(1).toDouble(&isok);
                if(isok)
                    gnss_->setAltitude(value);
                continue;
            }

            re.setPattern("^satTrCount, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setTrackingSatellites(value);
                continue;
            }


            re.setPattern("^SNR, (-?\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setSnr(value);
                continue;
            }

            re.setPattern("^SNRmask, (-?\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setSnrMask(value);
                continue;
            }

            re.setPattern("^elevationMask, (-?\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setElevationMask(value);
                continue;
            }

            //            selSys, Selected Systems, GPS Galileo, RW
            re.setPattern("^selSys, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto selectedSystems = value.split(" ");

                QList <int> systems;
                for(auto &sys : selectedSystems) {
                    if(sys.isEmpty())
                        continue;
                    systems.append(static_cast<int>(vch003request::satelliteSystemFromString(sys)));
                }

                gnss_->setSelectedSystems(systems);
                continue;
            }

            re.setPattern("^countGPS, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setNumberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::GPS), value);
                continue;
            }

            re.setPattern("^countGlonass, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setNumberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::GLONASS), value);
                continue;
            }

            re.setPattern("^countBeiDou, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setNumberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::BeiDou), value);
                continue;
            }

            re.setPattern("^countGalileo, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    gnss_->setNumberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::Galileo), value);
                continue;
            }

            // LED
            re.setPattern("^led(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 0 || index >= leds_->size()) // А тут индекс с 0.. (переделать бы в приборе)
                    continue;

                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;

                auto state = vch003request::ledStateFromString(value);

                if(index > 3 && index < 7) { //@N костыль..
                    auto channel = static_cast<InChannelGnssCard*>(inputChannels_[index - 4]);
                    if(!channel->configuration()) {
                        state.power = false;
                    }
                }

                leds_->at(index)->setState(state);
                continue;
            }
        }   // while
    }

    void GnssCard::parsePortSettings(int portIndex, const QString &parameterList)
    {
        auto port = portAt(portIndex);
        if(port == nullptr)
            return;

        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        bool isok;

        while(stream.readLineInto(&line)) {
            if(line.isEmpty()) continue;

            // Speed
            re.setPattern("^speed, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                port->setSpeed(vch003request::portSpeedFromString(match.capturedRef(1)));
                continue;
            }

            // MTU
            re.setPattern("^mtu, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    port->setMtu(value);
                continue;
            }

            re.setPattern("^swap, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                port->setSwap(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            re.setPattern("^loopback, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                port->setLoopback(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // Mac addr
            re.setPattern("^macAddress, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                port->setMacAddress( match.captured(1));
                continue;
            }

            re.setPattern("^ptpClock, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                port->setPpsSource(vch003request::ppsSourceFromString(match.capturedRef(1)));
                continue;
            }
        }

    }

    void GnssCard::parsePortStatistics(int portIndex, const QString &parameterList)
    {
        auto port = portAt(portIndex);
        if(port == nullptr)
            return;

        PortStatistics stat;

        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        bool isok;

        while(stream.readLineInto(&line)) {
            if(line.isEmpty()) continue;

            re.setPattern("^(.+), (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto name = match.capturedRef(1).toString();
                if(!name.isEmpty()) {
                    int value = match.capturedRef(2).toInt(&isok);
                    if(isok) {
                        if(name.compare("rxL2Good", Qt::CaseInsensitive)) {
                            stat.rxL2Good = value;
                        }
                        else if(name.compare("rxL2Fail", Qt::CaseInsensitive)) {
                            stat.rxL2Fail = value;
                        }
                        else if(name.compare("txL2MIIcounter", Qt::CaseInsensitive)) {
                            stat.txL2MIIcounter = value;
                        }
                        else if(name.compare("txL2Good", Qt::CaseInsensitive)) {
                            stat.txL2Good = value;
                        }
                        else if(name.compare("txL2Fail", Qt::CaseInsensitive)) {
                            stat.txL2Fail = value;
                        }
                        else if(name.compare("rxL2Bandwidth", Qt::CaseInsensitive)) {
                            stat.rxL2Bandwidth = value;
                        }
                        else if(name.compare("txL2Bandwidth", Qt::CaseInsensitive)) {
                            stat.txL2Bandwidth = value;
                        }
                        else if(name.compare("numberOfArpRxL2", Qt::CaseInsensitive)) {
                            stat.numberOfArpRxL2 = value;
                        }
                        else if(name.compare("numberOfArpTxL2", Qt::CaseInsensitive)) {
                            stat.numberOfArpTxL2 = value;
                        }
                        else if(name.compare("numberOfIcmpRxL2", Qt::CaseInsensitive)) {
                            stat.numberOfIcmpRxL2 = value;
                        }
                        else if(name.compare("numberOfIcmpTxL2", Qt::CaseInsensitive)) {
                            stat.numberOfIcmpTxL2 = value;
                        }
                        else if(name.compare("numberOfNtpRxL2", Qt::CaseInsensitive)) {
                            stat.numberOfNtpRxL2 = value;
                        }
                        else if(name.compare("numberOfNtpTxL2", Qt::CaseInsensitive)) {
                            stat.numberOfNtpTxL2 = value;
                        }
                        else if(name.compare("numberOfPtpRxL2", Qt::CaseInsensitive)) {
                            stat.numberOfPtpRxL2 = value;
                        }
                        else if(name.compare("numberOfPtpTxL2", Qt::CaseInsensitive)) {
                            stat.numberOfPtpTxL2 = value;
                        }
                        else if(name.compare("rxFromEthAnnounce", Qt::CaseInsensitive)) {
                            stat.rxFromEthAnnounce = value;
                        }
                        else if(name.compare("rxFromEthSync", Qt::CaseInsensitive)) {
                            stat.rxFromEthSync = value;
                        }
                        else if(name.compare("rxFromEthFollowUp", Qt::CaseInsensitive)) {
                            stat.rxFromEthFollowUp = value;
                        }
                        else if(name.compare("rxFromEthDelayReq", Qt::CaseInsensitive)) {
                            stat.rxFromEthDelayReq = value;
                        }
                        else if(name.compare("rxFromEthPDelayReq", Qt::CaseInsensitive)) {
                            stat.rxFromEthPDelayReq = value;
                        }
                        else if(name.compare("rxFromEthTLVsReq", Qt::CaseInsensitive)) {
                            stat.rxFromEthTLVsReq = value;
                        }
                        else if(name.compare("rxFromEthDelayResp", Qt::CaseInsensitive)) {
                            stat.rxFromEthDelayResp = value;
                        }
                        else if(name.compare("rxFromEthPDelayResp", Qt::CaseInsensitive)) {
                            stat.rxFromEthPDelayResp = value;
                        }
                        else if(name.compare("rxFromEthPDelayRespFollowUp", Qt::CaseInsensitive)) {
                            stat.rxFromEthPDelayRespFollowUp = value;
                        }
                        else if(name.compare("rxFromEthTLVsResp", Qt::CaseInsensitive)) {
                            stat.rxFromEthTLVsResp = value;
                        }


                        else if(name.compare("txFromEthAnnounce", Qt::CaseInsensitive)) {
                            stat.txFromEthAnnounce = value;
                        }
                        else if(name.compare("txFromEthSync", Qt::CaseInsensitive)) {
                            stat.txFromEthSync = value;
                        }
                        else if(name.compare("txFromEthFollowUp", Qt::CaseInsensitive)) {
                            stat.txFromEthFollowUp = value;
                        }
                        else if(name.compare("txFromEthDelayReq", Qt::CaseInsensitive)) {
                            stat.txFromEthDelayReq = value;
                        }
                        else if(name.compare("txFromEthPDelayReq", Qt::CaseInsensitive)) {
                            stat.txFromEthPDelayReq = value;
                        }
                        else if(name.compare("txFromEthTLVsReq", Qt::CaseInsensitive)) {
                            stat.txFromEthTLVsReq = value;
                        }
                        else if(name.compare("txFromEthDelayResp", Qt::CaseInsensitive)) {
                            stat.txFromEthDelayResp = value;
                        }
                        else if(name.compare("txFromEthPDelayResp", Qt::CaseInsensitive)) {
                            stat.txFromEthPDelayResp = value;
                        }
                        else if(name.compare("txFromEthPDelayRespFollowUp", Qt::CaseInsensitive)) {
                            stat.txFromEthPDelayRespFollowUp = value;
                        }
                        else if(name.compare("txFromEthTLVsResp", Qt::CaseInsensitive)) {
                            stat.txFromEthTLVsResp = value;
                        }
                    }
                }

                continue;
            }
        }

        port->setPortStatistics(stat);
    }

    void GnssCard::parseNetworkConfiguration(int portIndex, int confIndex, const QString &parameterList)
    {
        auto port = portAt(portIndex);
        if(port == nullptr)
            return;
        auto netConf = port->networkConfigurationAt(confIndex);
        if(netConf == nullptr)
            return;

        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        bool isok;

        while(stream.readLineInto(&line)) {
            if(line.isEmpty()) continue;

            // PORT

            re.setPattern("^ip4, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->setIp4(match.captured(1));
                continue;
            }

            re.setPattern("^mask, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->setMask(match.captured(1));
                continue;
            }

            re.setPattern("^gateway, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->setGateway(match.captured(1));
                continue;
            }

            // NTP

            re.setPattern("^stratum, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ntp()->setStratum(value);
                continue;
            }

            re.setPattern("^serverRefId, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->ntp()->setServerRefId(match.captured(1));
                continue;
            }


            // PTP

            re.setPattern("^ptpMode, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setPtpMode(vch003request::ptpModeFromString(value));
                continue;
            }

            re.setPattern("^txMode, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setTxMode(vch003request::txModeFromString(value));
                continue;
            }

            re.setPattern("^wayMode, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setWayMode(vch003request::wayModeFromString(value));
                continue;
            }

            re.setPattern("^syncMode, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setSyncMode(vch003request::syncModeFromString(value));
                continue;
            }

            re.setPattern("^delayMechanism, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setDelayMechanism(vch003request::delayMechanismFromString(value));
                continue;
            }

            re.setPattern("^protocol, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(!value.isEmpty())
                    netConf->ptp()->setProtocol(vch003request::protocolFromString(value));
                continue;
            }

            re.setPattern("^clockId, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->ptp()->setClockId(match.captured(1));
                continue;
            }

            re.setPattern("^domainNumber, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setDomainNumber(value);
                continue;
            }

            re.setPattern("^announceTimeout, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setAnnounceTimeout(value);
                continue;
            }
            re.setPattern("^syncInterval, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setSyncInterval(value);
                continue;
            }
            re.setPattern("^delayRequest, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setDelayRequest(value);
                continue;
            }
            re.setPattern("^pDelayRequest, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setPDelayRequest(value);
                continue;
            }
            re.setPattern("^priority1, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setPriority1(value);
                continue;
            }
            re.setPattern("^priority2, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setPriority2(value);
                continue;
            }
            re.setPattern("^clockClass, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    netConf->ptp()->setClockClass(value);
                continue;
            }
            re.setPattern("^remoteMasterAddress, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                netConf->ptp()->setRemoteMasterAddress(match.captured(1));
                continue;
            }
        }
    }

    QList<Request *> GnssCard::requestList() const
    {
        return Vch003Card::requestList();
    }

    QList<Request *> GnssCard::initCommandList() const
    {
        auto list = Vch003Card::initCommandList();
        list.append(updateSatelliteListCommand());

        list.append(new vch003request::GetGnssPortSettings(slot(), 0));
        list.append(new vch003request::GetGnssPortSettings(slot(), 1));

        list.append(new vch003request::GetGnssPortStatistics(slot(), 0));
        list.append(new vch003request::GetGnssPortStatistics(slot(), 1));

        for(int i = 0; i < port0()->networkConfigurationList().size(); ++i) {
            list.append(new vch003request::GetGnssNetworkConfiguration(slot(), 0, i));
            list.append(new vch003request::GetGnssNetworkConfiguration(slot(), 1, i));
        }

        return list;
    }

    SatelliteListModel *GnssCard::satelliteList() const
    {
        return satelliteList_;
    }
    
    int GnssCard::activeChannel() const
    {
        return activeChannel_;
    }
    
    void GnssCard::setActiveChannel(int channel)
    {
        if(activeChannel_ != channel) {
            activeChannel_ = channel;
            emit activeChannelChanged();
        }
    }
    
    int GnssCard::manualChannel() const
    {
        return manualChannel_;
    }
    
    void GnssCard::setManualChannel(int channel)
    {
        if(manualChannel_ != channel) {
            manualChannel_ = channel;
            emit manualChannelChanged();
        }
    }
    
    SSU::WorkingMode GnssCard::mode() const
    {
        return mode_;
    }
    
    void GnssCard::setMode(SSU::WorkingMode mode)
    {
        if(mode_ != mode) {
            mode_ = mode;
            emit modeChanged();
        }
    }

    SSU::PllState GnssCard::pllState() const
    {
        return pllState_;
    }

    void GnssCard::setPllState(SSU::PllState pllState)
    {
        if(pllState_ != pllState) {
            pllState_ = pllState;
            emit pllStateChanged();
        }
    }

    SSU::SSM GnssCard::ssm() const
    {
        return ssm_;
    }

    void GnssCard::setSsm(SSU::SSM ssm)
    {
        if(ssm_ != ssm) {
            ssm_ = ssm;
            emit ssmChanged();
        }
    }

    GnssModule *GnssCard::gnss() const
    {
        return gnss_;
    }
    
    bool GnssCard::ban() const
    {
        return ban_;
    }
    
    void GnssCard::setBan(bool ban)
    {
        if(ban_ != ban) {
            ban_ = ban;
            emit banChanged();
        }
    }

    EthernetPort *GnssCard::port0() const
    {
        return port0_;
    }

    EthernetPort *GnssCard::port1() const
    {
        return port1_;
    }

    bool GnssCard::measurementMode() const
    {
        return measurementMode_;
    }

    void GnssCard::setMeasurementMode(bool measurementMode)
    {
        if(measurementMode_ != measurementMode) {
            measurementMode_ = measurementMode;
            emit measurementModeChanged();
        }
    }

    int GnssCard::delayToRemoveBanByMeasurements() const
    {
        return delayToRemoveBanByMeasurements_;
    }

    void GnssCard::setDelayToRemoveBanByMeasurements(int delayToRemoveBanByMeasurements)
    {
        if(delayToRemoveBanByMeasurements_ != delayToRemoveBanByMeasurements) {
            delayToRemoveBanByMeasurements_ = delayToRemoveBanByMeasurements;
            emit delayToRemoveBanByMeasurementsChanged();
        }
    }

    int GnssCard::pllStateTimer() const
    {
        return pllStateTimer_;
    }

    void GnssCard::setPllStateTimer(int pllStateTimer)
    {
        if(pllStateTimer_ != pllStateTimer) {
            pllStateTimer_ = pllStateTimer;
            emit pllStateTimerChanged();
        }
    }

    int GnssCard::referenceGeneratorAddress() const
    {
        return referenceGeneratorAddress_;
    }

    void GnssCard::setReferenceGeneratorAddress(int referenceGeneratorAddress)
    {
        if(referenceGeneratorAddress_ != referenceGeneratorAddress) {
            referenceGeneratorAddress_ = referenceGeneratorAddress;
            emit referenceGeneratorAddressChanged();
        }
    }

    Request *GnssCard::updateSatelliteListCommand() const
    {
        return new vch003request::GetSatelliteList(slot());
    }


    Request *GnssCard::updatePortSettingsCommand(EthernetPort *port) const
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::GetGnssPortSettings(slot(), p);
    }

    Request *GnssCard::updatePortStatisticsCommand(EthernetPort *port) const
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::GetGnssPortStatistics(slot(), p);
    }

    Request *GnssCard::updateNetworkConfigurationCommand(EthernetPort *port, int confIndex) const
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::GetGnssNetworkConfiguration(slot(), p, confIndex);
    }

    Request *GnssCard::commandToSetWorkingMode(SSU::WorkingMode mode, int manualChannel)
    {
        if(manualChannel < 0 || manualChannel >= inputChannels_.size())
            return nullptr;
        auto str = vch003request::workingModeToString(mode, manualChannel);
        if(str.isEmpty())
            return nullptr;

        if(SSU::kChangedParamImmediately) {
            setMode(mode);
            setManualChannel(manualChannel);
        }

        return new vch003request::SetCardParameter(slot(), "mode", str);
    }

    Request *GnssCard::commandToSetActiveSignal(int index)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(!static_cast<InChannelGnssCard*>(inputChannels_[index])->configuration())  // Он не сконфигурирован..
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setActiveChannel(index);
        return mode_ == SSU::WorkingMode::AutoMode ? commandToSetChannelPriority(index, 0)
                                                   : commandToSetWorkingMode(SSU::WorkingMode::ManualMode, index);
    }

    Request *GnssCard::commandToSetChannelPriority(int index, int priority)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelGnssCard*>(inputChannels_[index])->setPriority(priority);
        return new vch003request::SetCardParameter(slot(), "priT", QString::number(index) + " " + QString::number(priority));
    }

    Request *GnssCard::commandToSetPriorities(const QList<int> &priorities)
    {
        if(priorities.size() != inputChannels_.size())
            return nullptr;
        QString arg;
        for(int i = 0; i < inputChannels_.size(); ++i) {
            if(SSU::kChangedParamImmediately)
                static_cast<InChannelGnssCard*>(inputChannels_[i])->setPriority(priorities.at(i));
            if(i != 0)
                arg.append(" ");
            arg.append(QString::number(priorities.at(i)));
        }
        return new vch003request::SetCardParameter(slot(), "priT", arg);
    }

    Request *GnssCard::commandToSetSsm(SSU::SSM ssm)
    {
        if(SSU::kChangedParamImmediately)
            setSsm(ssm);
        return new vch003request::SetCardParameter(slot(), "ssm", vch003request::ssmToString(ssm));
    }

    Request *GnssCard::commandToSetMeasurementMode(bool measurementMode)
    {
        if(SSU::kChangedParamImmediately)
            setMeasurementMode(measurementMode);
        return new vch003request::SetCardParameter(slot(), "measMode", measurementMode ? "ON" : "OFF");
    }

    Request *GnssCard::commandToSetDelayToRemoveBanByMeasurements(bool sec)
    {
        if(SSU::kChangedParamImmediately)
            setDelayToRemoveBanByMeasurements(sec);
        return new vch003request::SetCardParameter(slot(), "wtBan", QString::number(sec));
    }

    Request *GnssCard::commandToChannelConfiguration(int index, bool configuration)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelGnssCard*>(inputChannels_[index])->setConfiguration(configuration);
        return new vch003request::SetCardParameter(slot(), "power" + QString::number(index + 1), configuration ? "ON" : "OFF");
    }

    Request *GnssCard::commandToSetRestoreTime(int index, int sec)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelGnssCard*>(inputChannels_[index])->setWaitToRestoreTime(sec);
        return new vch003request::SetCardParameter(slot(), "wtRT" + QString::number(index + 1), QString::number(sec));
    }

    Request *GnssCard::commandToSetResetRestoreTimeout()
    {
        return new vch003request::SetCardParameter(slot(), "reswtRT");
    }

    Request *GnssCard::commandToSetPdopMask(double mask)
    {
        if(SSU::kChangedParamImmediately)
            gnss_->setPdopMask(mask);
        return new vch003request::SetCardParameter(slot(), "pdopMask", QString::number(mask, 'f', 6));
    }

    Request *GnssCard::commandToSetAntennaCableDelay(int delay)
    {
        if(SSU::kChangedParamImmediately)
            gnss_->setCableDelay(delay);
        return new vch003request::SetCardParameter(slot(), "antennaCableDelay", QString::number(delay));
    }

    Request *GnssCard::commandToSetSnr(int count)
    {
        if(SSU::kChangedParamImmediately)
            gnss_->setSnr(count);
        return new vch003request::SetCardParameter(slot(), "SNR", QString::number(count));
    }

    Request *GnssCard::commandToSetSnrMask(int mask)
    {
        if(SSU::kChangedParamImmediately)
            gnss_->setSnrMask(mask);
        return new vch003request::SetCardParameter(slot(), "SNRmask", QString::number(mask));
    }

    Request *GnssCard::commandToSetElevationMask(int mask)
    {
        if(SSU::kChangedParamImmediately)
            gnss_->setElevationMask(mask);
        return new vch003request::SetCardParameter(slot(), "elevationMask", QString::number(mask));
    }

    Request *GnssCard::commandToSelectSystems(const QList<int> &systems)
    {
        QString arg;
        for(int i = 0; i < systems.size(); ++i) {
            if(i != 0)
                arg.append(" ");
            arg.append(vch003request::satelliteSystemToString(static_cast<SSU::SatelliteSystem>(systems.at(i))));
        }
        if(SSU::kChangedParamImmediately)
            gnss_->setSelectedSystems(systems);
        return new vch003request::SetCardParameter(slot(), "selSys", arg);
    }

    Request *GnssCard::commandToSetPpsSource(EthernetPort *port, EthernetPort::PpsSource source)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " ptpClock",
                                                   vch003request::ppsSourceToString(source));
    }

    Request *GnssCard::commandToSetPortMacAddress(EthernetPort *port, QString value)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " macAddress",
                                                   value);
    }

    Request *GnssCard::commandToSetPortSpeed(EthernetPort *port, EthernetPort::Speed value)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " speed",
                                                   vch003request::portSpeedToString(value));
    }

    Request *GnssCard::commandToSetPortMtu(EthernetPort *port, int value)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " mtu",
                                                   QString::number(value));
    }

    Request *GnssCard::commandToSetPortSwap(EthernetPort *port, bool value)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " swap",
                                                   value ? "ON" : "OFF");
    }

    Request *GnssCard::commandToSetPortLoopback(EthernetPort *port, bool value)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "gnssport" + QString::number(p) + " loopback",
                                                   value ? "ON" : "OFF");
    }

    Request *GnssCard::networkConfigurationCommand(EthernetPort *port, int index, const QString &ip, const QString &ipMask, const QString &gateway)
    {
        if(index < 0 || index >= port->numberOfConfigurations())
            return nullptr;
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;

        return new vch003request::SetCardParameter(slot(),
                                                   "netconfig" + QString::number(p) + " " + QString::number(index) + " NET", { ip,
                                                                                                                               ipMask,
                                                                                                                               gateway });
    }

    Request *GnssCard::ntpConfigurationCommand(EthernetPort *port, int index, int stratum, const QString &serverRefId)
    {
        if(index < 0 || index >= port->numberOfConfigurations())
            return nullptr;
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(),
                                                   "netconfig" + QString::number(p) + " " + QString::number(index) + " NTP", { QString::number(stratum),
                                                                                                                               serverRefId });
    }

    Request *GnssCard::ptpConfigurationCommand(EthernetPort *port, int index,
                                               Ptp::TxMode txMode, Ptp::WayMode wayMode, Ptp::SyncMode syncMode,
                                               Ptp::DelayMechanism delayMech, int domainNumber, Ptp::Protocol protocol,
                                               Ptp::PtpMode ptpMode, const QString clockId,
                                               int announceTimeout, int syncInterval, int delayRequetInterval, int pDelayRequestInterval,
                                               int priority1, int priority2, int clockClass, const QString removeMasterAddress)
    {
        if(index < 0 || index >= port->numberOfConfigurations())
            return nullptr;
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::SetCardParameter(slot(),
                                                   "netconfig" + QString::number(p) + " " + QString::number(index) + " PTP", {
                                                       vch003request::txModeToString(txMode),
                                                       vch003request::wayModeToString(wayMode),
                                                       vch003request::syncModeToString(syncMode),
                                                       vch003request::delayMechanismToString(delayMech),
                                                       QString::number(domainNumber),
                                                       vch003request::protocolToString(protocol),
                                                       vch003request::ptpModeToString(ptpMode),
                                                       clockId,

                                                       QString::number(announceTimeout),
                                                       QString::number(syncInterval),
                                                       QString::number(delayRequetInterval),
                                                       QString::number(pDelayRequestInterval),

                                                       QString::number(priority1),
                                                       QString::number(priority2),
                                                       QString::number(clockClass),

                                                       removeMasterAddress
                                                   } );
    }

    Request *GnssCard::clearConfigurationCommand(EthernetPort *port, int index)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        auto conf = port->networkConfigurationAt(index);
        if(!conf->isValid())
            return nullptr;
        return new vch003request::ClearGnssPortConfiguration(slot(), p, index);
    }

    Request *GnssCard::clearNtpConfigurationCommand(EthernetPort *port, int index)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        auto conf = port->networkConfigurationAt(index);
        if(!conf->ntp()->isValid())
            return nullptr;
        return new vch003request::ClearNtpConfiguration(slot(), p, index);
    }

    Request *GnssCard::clearPtpConfigurationCommand(EthernetPort *port, int index)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        auto conf = port->networkConfigurationAt(index);
        if(!conf->ptp()->isValid())
            return nullptr;
        return new vch003request::ClearPtpConfiguration(slot(), p, index);
    }

    Request *GnssCard::clearPortStatisticsCommand(EthernetPort *port)
    {
        const int p = portNumber(port);
        if(p == -1)
            return nullptr;
        return new vch003request::ClearGnssPortStatistics(slot(), p);
    }

    int GnssCard::portNumber(EthernetPort *port) const
    {
        return port == nullptr ? -1
                               : port == port0_ ? 0
                                                : 1;
    }

    EthernetPort *GnssCard::portAt(int index) const
    {
        switch (index) {
            case 0: return port0_;
            case 1: return port1_;
            default: return nullptr;
        }
    }

}

