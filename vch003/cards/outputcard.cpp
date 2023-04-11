#include "outputcard.h"
#include "vch003/configuration/vch003options.h"

#include "vch003/channels/outchanneloutputcard.h"
#include "vch003/channels/inchanneloutputcard.h"

#include <QRegularExpression>
#include <QTextStream>

namespace ssu {

    OutputCard::OutputCard(int address, QObject *parent) : Vch003Card(SSU::Output, address, parent),
        active_(true),
        synchronization_(true),
        activeChannel_(0),
        backupChannel_(1),
        ssmControlMode_(Vch003::SsmControlMode::Auto),
        ssm_(SSU::SSM::None),
        ssmRai_(true),
        enableSSMTable_(true),
        enableBypass_(true),
        enableOutputChannels_(true)
    {
        leds_ = new LedList({
                                Qt::darkGreen,      // Питание
                                Qt::darkGreen,      // ЗГ 1
                                Qt::darkGreen,      // ЗГ 2
                                Qt::darkGreen,      // Синхронизация
                                Qt::darkGreen,      // Актив
                                Qt::red,            // Авария
                                Qt::darkGreen       // Обход
                            },
                            this);

        for(int i = 0; i < Vch003DeviceOptions::kNumberOfOutputCardInChannels; ++i) {
            auto channel = new InChannelOutputCard(this);
            //        channel->setPriority(i);
            channel->setCurrentPriority(i);
            inputChannels_.append(channel);
        }

        for(int i = 0; i < Vch003DeviceOptions::kNumberOfOutputCardOutChannels; ++i)
            outputChannels_.append(new OutChannelOutputCard(this));

        if(SSU::kImitationWork) {
            //        enableBypass_ = false;
            //        enableOutputChannels_ = false;
            //        synchronization_ = false;
            activeChannel_ = 16;
            backupChannel_ = 17;
            static_cast<OutChannelOutputCard*>(outputChannels_[11])->setType(SSU::SignalType::KHz2048);
        }
    }

    void OutputCard::parseParameters(const QString &parameterList)
    {
        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;
        bool isok;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        while(stream.readLineInto(&line)) {
            if(line.isEmpty())
                continue;

            // Mode
            re.setPattern("^mode, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setActive(vch003request::activeCardFromString(value));
                continue;
            }

            // Sync
            re.setPattern("^sync, (0|1)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setSynchronization(value != "0");
                continue;
            }

            // enableOutputChannels
            re.setPattern("^enableOuts, (0|1)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableOutputChannels(value != "0");
                continue;
            }

            // Type_Inp
            re.setPattern("^typ(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > outputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<OutChannelOutputCard*>(outputChannels_[index - 1])->setType(vch003request::signalTypeFromString(value));
                continue;
            }

            re.setPattern("^enableOutAtHoldover, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto enableOutAtHoldover = value.split(" ");
                for(int i = 0; i < enableOutAtHoldover.size(); ++i) {
                    if(i >= outputChannels_.size()) break;
                    bool enable = enableOutAtHoldover.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<OutChannelOutputCard*>(outputChannels_[i])->setHoldoverModeEnabled(enable);
                }
                continue;
            }

            re.setPattern("^stateOut, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto states = value.split(" ");
                for(int i = 0; i < states.size(); ++i) {
                    if(i >= outputChannels_.size()) break;
                    bool state = states.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<OutChannelOutputCard*>(outputChannels_[i])->setState(state);
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

            // SSM auto
            re.setPattern("^ssmAuto, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setSsmControlMode(value.compare(QString("ON"), Qt::CaseInsensitive) == 0 ? Vch003::SsmControlMode::Auto
                                                                                         : Vch003::SsmControlMode::Manual);
                continue;
            }

            // ssmRAI
            re.setPattern("^ssmRAI, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setSsmRai(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // enableSSM
            re.setPattern("^enableSSM, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableSSMTable(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // enableBypass
            re.setPattern("^enableBypass, (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableBypass(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // enableOuts
            re.setPattern("^enableOuts, (0|1)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableOutputChannels(value != "0");
                continue;
            }

            // Active chanel number
            re.setPattern("^actH, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    setActiveChannel(value);
                continue;
            }

            // Backup chanel number
            re.setPattern("^bkpH, (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1).toInt(&isok);
                if(isok)
                    setBackupChannel(value);
                continue;
            }

            // Chanels table of bans
            re.setPattern("^banS, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto banS = value.split(" ");
                for(int i = 0; i < banS.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    bool ban = banS.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelOutputCard*>(inputChannels_[i])->setBan(ban);
                }
                continue;
            }

            // Home table
            re.setPattern("^homeS, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto homeS = value.split(" ");
                for(int i = 0; i < homeS.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    bool home = homeS.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelOutputCard*>(inputChannels_[i])->setHome(home);
                }
                continue;
            }

            // Chanels table of priorities
            re.setPattern("^priBypass, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto priBypass = value.split(" ");
                for(int i = 0; i < priBypass.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    int pri = priBypass.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelOutputCard*>(inputChannels_[i])->setPriority(pri);
                }
                continue;
            }

            re.setPattern("^priCurrS, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                auto priCurrS = value.split(" ");
                for(int i = 0; i < priCurrS.size(); ++i) {
                    if(i >= inputChannels_.size()) break;

                    int pri = priCurrS.at(i).toInt(&isok);
                    if(!isok) break;
                    static_cast<InChannelOutputCard*>(inputChannels_[i])->setCurrentPriority(pri);
                }
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
        }
    }

    bool OutputCard::active() const
    {
        return active_;
    }

    void OutputCard::setActive(bool active)
    {
        if(active_ != active) {
            active_ = active;
            emit activeChanged();
        }
    }

    bool OutputCard::synchronization() const
    {
        return synchronization_;
    }

    void OutputCard::setSynchronization(bool synchronization)
    {
        if(synchronization_ != synchronization) {
            synchronization_ = synchronization;
            emit synchronizationChanged();
        }
    }

    int OutputCard::activeChannel() const
    {
        return activeChannel_;
    }

    void OutputCard::setActiveChannel(int activeChannel)
    {
        if(activeChannel < 0) return;
        if(activeChannel_ != activeChannel) {
            activeChannel_ = activeChannel;
            emit activeChannelChanged();
        }
    }

    int OutputCard::backupChannel() const
    {
        return backupChannel_;
    }

    void OutputCard::setBackupChannel(int backupChannel)
    {
        if(backupChannel < 0) return;
        if(backupChannel_ != backupChannel) {
            backupChannel_ = backupChannel;
            emit backupChannelChanged();
        }
    }

    Vch003::SsmControlMode OutputCard::ssmControlMode() const
    {
        return ssmControlMode_;
    }

    void OutputCard::setSsmControlMode(Vch003::SsmControlMode ssmControlMode)
    {
        if(ssmControlMode_ != ssmControlMode) {
            ssmControlMode_ = ssmControlMode;
            emit ssmControlModeChanged();
        }
    }

    SSU::SSM OutputCard::ssm() const
    {
        return ssm_;
    }

    void OutputCard::setSsm(SSU::SSM ssm)
    {
        if(ssm_ != ssm) {
            ssm_ = ssm;
            emit ssmChanged();
        }
    }

    bool OutputCard::ssmRai() const
    {
        return ssmRai_;
    }

    void OutputCard::setSsmRai(bool ssmRai)
    {
        if(ssmRai_ != ssmRai) {
            ssmRai_ = ssmRai;
            emit ssmRaiChanged();
        }
    }

    bool OutputCard::enableSSMTable() const
    {
        return enableSSMTable_;
    }

    void OutputCard::setEnableSSMTable(bool enableSSMTable)
    {
        if(enableSSMTable_ != enableSSMTable) {
            enableSSMTable_ = enableSSMTable;
            emit enableSSMTableChanged();
        }
    }

    bool OutputCard::enableBypass() const
    {
        return enableBypass_;
    }

    void OutputCard::setEnableBypass(bool enableBypass)
    {
        if(enableBypass_ != enableBypass) {
            enableBypass_ = enableBypass;
            emit enableBypassChanged();
        }
    }

    Request *OutputCard::commandToGiveActivity()
    {
        if(!active_)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setActive(false);
        return new vch003request::SetCardParameter(slot(), "mode", vch003request::activeCardToString(false));
    }

    Request *OutputCard::commandToSetTypeOutputChannel(int index, SSU::SignalType type)
    {
        if(index < 0 || index >= outputChannels_.size())
            return nullptr;

        auto channel = static_cast<OutChannelOutputCard*>(outputChannels_[index]);
        if(channel->type() == type)
            return nullptr;

        if(SSU::kChangedParamImmediately)
            channel->setType(type);
        return new vch003request::SetCardParameter(slot(), "typ" + QString::number(index + 1), vch003request::signalTypeToString(type));
    }

    Request *OutputCard::commandToSwitchToBackupGenerator()
    {
        return new vch003request::SetCardParameter(slot(), "swAct");
    }

    Request *OutputCard::commandToSetPriorityInputSignal(int index, int priority)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelOutputCard*>(inputChannels_[index])->setPriority(priority);
        return new vch003request::SetCardParameter(slot(), "priBypass", QString::number(index) + " " + QString::number(priority));
    }

    Request *OutputCard::commandToSetPriorities(const QList<int> &priorities)
    {
        QString arg;
        for(int i = 0; i < priorities.size(); ++i) {
            if(SSU::kChangedParamImmediately)
                static_cast<InChannelOutputCard*>(inputChannels_[i])->setPriority(priorities.at(i));
            if(i != 0)
                arg.append(",");
            arg.append(QString::number(priorities.at(i)));
        }
        return new vch003request::SetCardParameter(slot(), "priBypass", arg);
    }

    Request *OutputCard::commandToSetSsmControlMode(Vch003::SsmControlMode ssmControlMode)
    {
        if(ssmControlMode_ == ssmControlMode)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setSsmControlMode(ssmControlMode);
        return new vch003request::SetCardParameter(slot(), "ssmAuto", ssmControlMode == Vch003::SsmControlMode::Auto ? "ON" : "OFF");
    }

    Request *OutputCard::commandToSetSsmRai(bool ssmRai)
    {
        if(ssmRai_ == ssmRai)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setSsmRai(ssmRai);
        return new vch003request::SetCardParameter(slot(), "ssmRAI", ssmRai ? "ON" : "OFF");
    }

    Request *OutputCard::commandToSetSsm(SSU::SSM ssm)
    {
        if(ssm_ == ssm)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setSsm(ssm);
        return new vch003request::SetCardParameter(slot(), "ssm", vch003request::ssmToString(ssm));
    }

    Request *OutputCard::enableSSMTableCommand(bool enable)
    {
        if(enableSSMTable_ == enable)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setEnableSSMTable(enable);
        return new vch003request::SetCardParameter(slot(), "enableSSM", enable ? "ON" : "OFF");
    }

    Request *OutputCard::enableBypassCommand(bool enable)
    {
        if(enableBypass_ == enable)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setEnableBypass(enable);
        return new vch003request::SetCardParameter(slot(), "enableBypass", enable ? "ON" : "OFF");
    }

    Request *OutputCard::commandToSetPermissionOnHoldover(int index, bool check)
    {
        if(index < 0 || index >= outputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<OutChannelOutputCard*>(outputChannels_[index])->setHoldoverModeEnabled(check);
        return new vch003request::SetCardParameter(slot(), "enableOutAtHoldover " + QString::number(index), QString::number(check ? 1 : 0));
    }

    bool OutputCard::enableOutputChannels() const
    {
        return enableOutputChannels_;
    }

    void OutputCard::setEnableOutputChannels(bool enableOutputChannels)
    {
        if(enableOutputChannels_ != enableOutputChannels) {
            enableOutputChannels_ = enableOutputChannels;
            emit enableOutputChannelsChanged();
        }
    }
}
