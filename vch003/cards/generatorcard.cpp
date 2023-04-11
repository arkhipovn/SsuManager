#include "generatorcard.h"
#include "vch003/channels/inchannelgeneratorcard.h"
#include "ssu.h"
#include "vch003/configuration/vch003options.h"

#include <QList>
#include <QRegularExpression>
#include <QTextStream>

namespace ssu {
    GeneratorCard::GeneratorCard(int address, QObject *parent) : Vch003Card(SSU::Generator, address, parent),
        generatorType_(SSU::GeneratorType::Quartz),
        master_(false),
        ssuType_(SSU::SsuType::SSU_A),
        pllState_(SSU::PllState::Warmup),
        pllStateTimer_(0),
        activeChannel_(0),
        manualChannel_(0),
        mode_(SSU::WorkingMode::AutoMode),
        error_(0),
        ssm_(SSU::SSM::None),
        //    enableCopyPriority_(true),
        enableSSMTable_(true)
    {
        leds_ = new LedList({
                                Qt::darkGreen,      // Питание
                                Qt::yellow,         // Прогрев
                                Qt::darkGreen,      // Захват/Удержание
                                Qt::red             // Авария
                            },
                            this);

        for(int i = 0; i < Vch003DeviceOptions::kNumberOfGeneratorInChannels; ++i) {
            auto channel = new InChannelGeneratorCard(this);
            channel->setCurrentPriority(i);
            inputChannels_.append(channel);
        }

        if(SSU::kImitationWork) {
            pllStateTimer_ = 300;
            pllState_ = SSU::PllState::Tracking;
            activeChannel_ = 16;
            static_cast<InChannelGeneratorCard*>(inputChannels_[1])->setBan(true);
            static_cast<InChannelGeneratorCard*>(inputChannels_[2])->setBan(true);
            static_cast<InChannelGeneratorCard*>(inputChannels_[3])->setPriority(3);
            static_cast<InChannelGeneratorCard*>(inputChannels_[9])->setPriority(5);
            error_ = Vch003::GeneratorError::ControlLoopFailure | Vch003::GeneratorError::GeneratorFailure;
        }
    }

    void GeneratorCard::parseParameters(const QString &parameterList)
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

            re.setPattern("^isMaster, (true|false)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setMaster(value.compare(QString("true"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // Mode
            re.setPattern("^gmode, ([^-]+)-?(\\d+)?");
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

            // Chanels table of bans
            re.setPattern("^banT, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto banT = value.split(" ");
                for(int i = 0; i < banT.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    bool ban = banT.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelGeneratorCard*>(inputChannels_[i])->setBan(ban);
                }
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
                    static_cast<InChannelGeneratorCard*>(inputChannels_[i])->setPriority(pri);
                }
                continue;
            }

            re.setPattern("^priCurrT, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto priT = value.split(" ");
                for(int i = 0; i < priT.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    int pri = priT.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelGeneratorCard*>(inputChannels_[i])->setCurrentPriority(pri);
                }
                continue;
            }

            // Error
            re.setPattern("^error, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto error = match.capturedRef(1).toInt(&isok);
                if(isok)
                    setError(error);
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

            re.setPattern("^enableSSM, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableSSMTable(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }


            // stateTimerCounter
            re.setPattern("^stateTimerCounter, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    setPllStateTimer(value);
                continue;
            }

            re.setPattern("^typeUSS, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setSsuType(vch003request::ssuTypeFromString(value));
                continue;
            }

            re.setPattern("^genType, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setGeneratorType(vch003request::generatorTypeFromString(value));
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
                leds_->at(index)->setState(vch003request::ledStateFromString(value));
                continue;
            }

        }   // while
    }

    int GeneratorCard::activeChannel() const
    {
        return activeChannel_;
    }

    void GeneratorCard::setActiveChannel(int channel)
    {
        if(activeChannel_ != channel) {
            activeChannel_ = channel;
            emit activeChannelChanged();
        }
    }

    int GeneratorCard::manualChannel() const
    {
        return manualChannel_;
    }

    void GeneratorCard::setManualChannel(int channel)
    {
        if(manualChannel_ != channel) {
            manualChannel_ = channel;
            emit manualChannelChanged();
        }
    }

    SSU::WorkingMode GeneratorCard::mode() const
    {
        return mode_;
    }

    void GeneratorCard::setMode(SSU::WorkingMode mode)
    {
        if(mode_ != mode) {
            mode_ = mode;
            emit modeChanged();
        }
    }

    SSU::PllState GeneratorCard::pllState() const
    {
        return pllState_;
    }

    void GeneratorCard::setPllState(SSU::PllState pllState)
    {
        if(pllState_ != pllState) {
            pllState_ = pllState;
            emit pllStateChanged();
        }
    }

    SSU::SSM GeneratorCard::ssm() const
    {
        return ssm_;
    }

    void GeneratorCard::setSsm(SSU::SSM ssm)
    {
        if(ssm_ != ssm) {
            ssm_ = ssm;
            emit ssmChanged();
        }
    }

    unsigned char GeneratorCard::error() const
    {
        return error_;
    }

    void GeneratorCard::setError(unsigned char error)
    {
        if(error_ != error) {
            error_ = error;
            emit errorChanged();
        }
    }

    bool GeneratorCard::enableSSMTable() const
    {
        return enableSSMTable_;
    }

    void GeneratorCard::setEnableSSMTable(bool enableSSMTable)
    {
        if(enableSSMTable_ != enableSSMTable) {
            enableSSMTable_ = enableSSMTable;
            emit enableSSMTableChanged();
        }
    }

    SSU::GeneratorType GeneratorCard::generatorType() const
    {
        return generatorType_;
    }

    void GeneratorCard::setGeneratorType(const SSU::GeneratorType &type)
    {
        if(generatorType_ != type) {
            generatorType_ = type;
            emit generatorTypeChanged();
        }
    }

    int GeneratorCard::pllStateTimer() const
    {
        return pllStateTimer_;
    }

    void GeneratorCard::setPllStateTimer(int pllStateTimer)
    {
        if(pllStateTimer_ != pllStateTimer) {
            pllStateTimer_ = pllStateTimer;
            emit pllStateTimerChanged();
        }
    }

    bool GeneratorCard::isMaster() const
    {
        return master_;
    }

    void GeneratorCard::setMaster(bool master)
    {
        if(master_ != master) {
            master_ = master;
            emit masterChanged();
        }
    }

    SSU::SsuType GeneratorCard::ssuType() const
    {
        return ssuType_;
    }

    void GeneratorCard::setSsuType(const SSU::SsuType &ssuType)
    {
        if(ssuType_ != ssuType) {
            ssuType_ = ssuType;
            emit ssuTypeChanged();
        }
    }


    Request *GeneratorCard::commandToSetWorkingMode(SSU::WorkingMode mode, int manualChannel)
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

        return new vch003request::SetCardParameter(slot(), "gmode", str);
    }

    Request *GeneratorCard::commandToSetActiveSignal(int index)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(static_cast<InChannelGeneratorCard*>(inputChannels_[index])->ban())  // Он в бане..
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setActiveChannel(index);
        return commandToSetChannelPriority(index, 0);
    }

    Request *GeneratorCard::commandToChangeActiveSlot()
    {
        return new vch003request::SetCardParameter(slot(), "swActSlot");
    }

    Request *GeneratorCard::commandToSetChannelPriority(int index, int priority)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelGeneratorCard*>(inputChannels_[index])->setPriority(priority);
        return new vch003request::SetCardParameter(slot(), "priT", QString::number(index) + " " + QString::number(priority));
    }

    Request *GeneratorCard::commandToSetPriorities(const QList<int> &priorities)
    {
        if(priorities.size() != inputChannels_.size())
            return nullptr;
        QString arg;
        for(int i = 0; i < inputChannels_.size(); ++i) {
            if(SSU::kChangedParamImmediately)
                static_cast<InChannelGeneratorCard*>(inputChannels_[i])->setPriority(priorities.at(i));
            if(i != 0)
                arg.append(" ");
            arg.append(QString::number(priorities.at(i)));
        }
        return new vch003request::SetCardParameter(slot(), "priT", arg);
    }

    Request *GeneratorCard::enableSSMTableCommand(bool enable)
    {
        //    if(SSU::kChangedParamImmediately)
        //        setEnableSSMTable(enable);
        return new vch003request::SetCardParameter(slot(), "enableSSM", enable ? "ON" : "OFF");
    }

    Request *GeneratorCard::commandToSetSsuType(SSU::SsuType type)
    {
        if(SSU::kChangedParamImmediately)
            setSsuType(type);
        return new vch003request::SetCardParameter(slot(), "typeUSS", vch003request::ssuTypeToString(type));
    }

}



