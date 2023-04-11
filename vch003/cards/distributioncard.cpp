#include "distributioncard.h"
#include "vch003/configuration/vch003options.h"
#include "vch003/channels/outchanneloutputcard.h"

#include <QRegularExpression>
#include <QTextStream>

namespace ssu {
    DistributionCard::DistributionCard(int address, QObject *parent) : Vch003Card(SSU::Distribution, address, parent),
        active_(true),
        synchronization_(true),
        enableOutputChannels_(true)
    {
        leds_ = new LedList({
                                Qt::darkGreen,      // Питание
                                Qt::darkGreen,      // Синхронизация
                                Qt::darkGreen,      // Актив
                                Qt::red             // Авария
                            },
                            this);

        for(int i = 0; i < Vch003DeviceOptions::kNumberOfOutputCardOutChannels; ++i)
            outputChannels_.append(new OutChannelOutputCard(this));

        if(SSU::kImitationWork) {
            static_cast<OutChannelOutputCard*>(outputChannels_[11])->setType(SSU::SignalType::KHz2048);
            synchronization_ = false;
        }
    }

    void DistributionCard::parseParameters(const QString &parameterList)
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

            // enableOuts
            re.setPattern("^enableOuts, (0|1)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setEnableOutputChannels(value != "0");
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

    bool DistributionCard::active() const
    {
        return active_;
    }

    void DistributionCard::setActive(bool active)
    {
        if(active_ != active) {
            active_ = active;
            emit activeChanged();
        }
    }

    bool DistributionCard::synchronization() const
    {
        return synchronization_;
    }

    void DistributionCard::setSynchronization(bool synchronization)
    {
        if(synchronization_ != synchronization) {
            synchronization_ = synchronization;
            emit synchronizationChanged();
        }
    }

    Request *DistributionCard::commandToGiveActivity()
    {
        if(!active_)
            return nullptr;
        if(SSU::kChangedParamImmediately)
            setActive(false);
        return new vch003request::SetCardParameter(slot(), "mode", vch003request::activeCardToString(false));
    }

    Request *DistributionCard::commandToSetTypeOutputChannel(int index, SSU::SignalType type)
    {
        if(index < 0 || index >= outputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<OutChannelOutputCard*>(outputChannels_[index])->setType(type);
        return new vch003request::SetCardParameter(slot(), "typ" + QString::number(index + 1), vch003request::signalTypeToString(type));
    }

    Request *DistributionCard::commandToSetPermissionOnHoldover(int index, bool check)
    {
        if(index < 0 || index >= outputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<OutChannelOutputCard*>(outputChannels_[index])->setHoldoverModeEnabled(check);
        return new vch003request::SetCardParameter(slot(), "enableOutAtHoldover " + QString::number(index), QString::number(check ? 1 : 0));
    }

    bool DistributionCard::enableOutputChannels() const
    {
        return enableOutputChannels_;
    }

    void DistributionCard::setEnableOutputChannels(bool enableOutputChannels)
    {
        if(enableOutputChannels_ != enableOutputChannels) {
            enableOutputChannels_ = enableOutputChannels;
            emit enableOutputChannelsChanged();
        }
    }
}
