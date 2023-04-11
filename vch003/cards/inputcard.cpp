#include "inputcard.h"
#include "ssu.h"
#include "vch003/configuration/vch003options.h"

#include "vch003/channels/inchannelinputcard.h"
#include <QRegularExpression>
#include <QTextStream>

namespace ssu {

    InputCard::InputCard(int address, QObject *parent) : Vch003Card(SSU::Input, address, parent),
        master_(false)
    {
        leds_ = new LedList({
                                Qt::darkGreen,     // Питание
                                Qt::darkGreen,     // Вход 1
                                Qt::darkGreen,     // Вход 2
                                Qt::darkGreen,     // Вход 3
                                Qt::darkGreen,     // Вход 4
                                Qt::red            // Авария
                            },
                            this);

        for(int i = 0; i < Vch003DeviceOptions::kNumberOfInputCardInChannels; ++i)
            inputChannels_.append(new InChannelInputCard(this));

        if(SSU::kImitationWork) {
            static_cast<InChannelInputCard*>(inputChannels_[0])->setType(SSU::SignalType::Off);
            static_cast<InChannelInputCard*>(inputChannels_[0])->setE1bit(Vch003::E1Bit::Sa5);
            static_cast<InChannelInputCard*>(inputChannels_[1])->setType(SSU::SignalType::E1);

            static_cast<InChannelInputCard*>(inputChannels_[2])->setType(SSU::SignalType::MHz10);
            static_cast<InChannelInputCard*>(inputChannels_[2])->setBan(true);

            static_cast<InChannelInputCard*>(inputChannels_[3])->setType(SSU::SignalType::E1);
            static_cast<InChannelInputCard*>(inputChannels_[3])->setState(Vch003::RecoveryDelay);

            static_cast<InChannelInputCard*>(inputChannels_[0])->setFailureTransmEndSensitivity(Vch003::Sensitivity::Off);
            static_cast<InChannelInputCard*>(inputChannels_[0])->setCrcErrorsSensitivity(Vch003::Sensitivity::Sensitivity0);
            static_cast<InChannelInputCard*>(inputChannels_[0])->setLossOfMultiFrameSensitivity(Vch003::Sensitivity::Sensitivity1);
            static_cast<InChannelInputCard*>(inputChannels_[0])->setLossOfFrameSensitivity(Vch003::Sensitivity::Sensitivity2);
            static_cast<InChannelInputCard*>(inputChannels_[0])->setHdb3ErrorSensitivity(Vch003::Sensitivity::Sensitivity3);

            static_cast<InChannelInputCard*>(inputChannels_[1])->setMeasurementMode(true);
        }
    }

    bool InputCard::isMaster() const
    {
        return master_;
    }

    void InputCard::setMaster(bool master)
    {
        if(master_ != master) {
            master_ = master;
            emit masterChanged();
        }
    }

    void InputCard::parseParameters(const QString &parameterList)
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

            re.setPattern("^isMaster, (true|false)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                if(value.isEmpty()) continue;
                setMaster(value.compare(QString("true"), Qt::CaseInsensitive) == 0);
                continue;
            }

            // Type_Inp
            re.setPattern("^typ(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setType(vch003request::signalTypeFromString(value));
                continue;
            }

            // Stat_Threshold_Inp
            re.setPattern("^STS(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setStatusThreshold(vch003request::statusThresholdFromString(value));
                continue;
            }

            // SSM
            re.setPattern("^ssm(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setSsm(vch003request::ssmFromString(value));
                continue;
            }

            // Restore Time
            re.setPattern("^wtRT(\\d+), (\\d+)");  // (.+)
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                int value = match.capturedRef(2).toInt(&isok);
                if(!isok) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setWaitToRestoreTime(value);
                continue;
            }

            // State
            re.setPattern("^sti(\\d+), (\\d+)");   //(.+)
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                int value = match.capturedRef(2).toInt(&isok);
                if(!isok) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setState(vch003request::inputChannelState(value));
                continue;
            }

            re.setPattern("meaM(\\d+), (OFF|ON)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setMeasurementMode(value.compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            re.setPattern("^Elsb(\\d+), (\\d+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                int value = match.capturedRef(2).toInt(&isok);
                if(!isok) continue;
                auto channel = static_cast<InChannelInputCard*>(inputChannels_[index - 1]);

                channel->setFailureTransmEndSensitivity(vch003request::sensitivityFromRawData(value));
                channel->setCrcErrorsSensitivity(vch003request::sensitivityFromRawData(value >> 3));
                channel->setLossOfMultiFrameSensitivity(vch003request::sensitivityFromRawData(value >> 6));
                channel->setLossOfFrameSensitivity(vch003request::sensitivityFromRawData(value >> 9));
                channel->setHdb3ErrorSensitivity(vch003request::sensitivityFromRawData(value >> 12, false));
                continue;
            }

            re.setPattern("^ssmB(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 1 || index > inputChannels_.size()) // Индекс начинается с 1
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                static_cast<InChannelInputCard*>(inputChannels_[index - 1])->setE1bit(vch003request::e1bitFromString(value));
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

                if(index > 0 && index < 5) {    // 0ая лампочка - это питание.. Поэтому > 0, а дальше вычитаем 1...
                    auto channel = static_cast<InChannelInputCard*>(inputChannels_[index - 1]);

                    //@N костыль.. Тк нельзя считать забанен сигнал или нет..
                    bool isRed = state.color == Qt::red;
                    channel->setBan(isRed);

                    if(!isRed) {
                        if(channel->type() == SSU::SignalType::Auto) {
                            state.blinkInterval = 4000;
                        }
                        else if(channel->measurementMode()) {
                            state.blinkInterval = 2000;
                        }
                        else if(state.color == Qt::yellow) {
                            channel->setState(channel->state() | Vch003::InputSignalState::RecoveryDelay);
                        }
                    }
                }

                leds_->at(index)->setState(state);

                continue;
            }
        }
    }

    Request *InputCard::commandToSetTypeChannel(int index, SSU::SignalType type)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setType(type);
        return new vch003request::SetCardParameter(slot(), "typ" + QString::number(index + 1), vch003request::signalTypeToString(type));
    }

    Request *InputCard::commandForAutodetectingChannelType(int index)
    {
        return commandToSetTypeChannel(index, SSU::SignalType::Auto);
    }

    Request *InputCard::commandToSetThresholdStatus(int index, SSU::StatusThreshold status)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setStatusThreshold(status);
        return new vch003request::SetCardParameter(slot(), "STS" + QString::number(index + 1), vch003request::statusThresholdToString(status));
    }

    Request *InputCard::commandToSetSsm(int index, SSU::SSM ssm)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setSsm(ssm);
        return new vch003request::SetCardParameter(slot(), "ssm" + QString::number(index + 1), vch003request::ssmToString(ssm));
    }

    Request *InputCard::commandToSetRestoreTime(int index, int sec)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setWaitToRestoreTime(sec);
        return new vch003request::SetCardParameter(slot(), "wtRT" + QString::number(index + 1), QString::number(sec));
    }

    Request *InputCard::commandToSetResetRestoreTimeout(int index)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        return new vch003request::SetCardParameter(slot(), "reswtRT" + QString::number(index + 1));
    }

    Request *InputCard::commandToSetSensitivity(int index,
                                                Vch003::Sensitivity failureTransmEndSensitivity,
                                                Vch003::Sensitivity crcErrorsSensitivity,
                                                Vch003::Sensitivity lossOfMultiFrameSensitivity,
                                                Vch003::Sensitivity lossOfFrameSensitivity,
                                                Vch003::Sensitivity hdb3ErrorSensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;

        auto channel = static_cast<InChannelInputCard*>(inputChannels_[index]);

        int value = vch003request::sensitivityToRawData(failureTransmEndSensitivity) |
                (vch003request::sensitivityToRawData(crcErrorsSensitivity) << 3) |
                (vch003request::sensitivityToRawData(lossOfMultiFrameSensitivity) << 6) |
                (vch003request::sensitivityToRawData(lossOfFrameSensitivity) << 9) |
                (vch003request::sensitivityToRawData(hdb3ErrorSensitivity, false) << 12);

        if(SSU::kChangedParamImmediately) {
            channel->setFailureTransmEndSensitivity(failureTransmEndSensitivity);
            channel->setCrcErrorsSensitivity(crcErrorsSensitivity);
            channel->setLossOfMultiFrameSensitivity(lossOfMultiFrameSensitivity);
            channel->setLossOfFrameSensitivity(lossOfFrameSensitivity);
            channel->setHdb3ErrorSensitivity(hdb3ErrorSensitivity);
        }

        return new vch003request::SetCardParameter(slot(), "Elsb" + QString::number(index + 1), QString::number(value));
    }

    Request *InputCard::commandToSetSensitivityToFailureAtTheTransmEnd(int index, Vch003::Sensitivity sensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setFailureTransmEndSensitivity(sensitivity);
        return new vch003request::SetCardParameter(slot(), "sbATE" + QString::number(index + 1), vch003request::sensitivityToString(sensitivity));
    }

    Request *InputCard::commandToSetSensitivityToCrcErrors(int index, Vch003::Sensitivity sensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setCrcErrorsSensitivity(sensitivity);
        return new vch003request::SetCardParameter(slot(), "sbECRC" + QString::number(index + 1), vch003request::sensitivityToString(sensitivity));
    }

    Request *InputCard::commandToSetSensitivityToLossOfMultiframeSync(int index, Vch003::Sensitivity sensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setLossOfMultiFrameSensitivity(sensitivity);
        return new vch003request::SetCardParameter(slot(), "sbEMC" + QString::number(index + 1), vch003request::sensitivityToString(sensitivity));
    }

    Request *InputCard::commandToSetSensitivityToLossOfFrameSync(int index, Vch003::Sensitivity sensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setLossOfFrameSensitivity(sensitivity);
        return new vch003request::SetCardParameter(slot(), "sbECS" + QString::number(index + 1), vch003request::sensitivityToString(sensitivity));
    }

    Request *InputCard::commandToSetSensitivityHdb3coddingError(int index, Vch003::Sensitivity sensitivity)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setHdb3ErrorSensitivity(sensitivity);
        return new vch003request::SetCardParameter(slot(), "sbEHDB" + QString::number(index + 1), vch003request::sensitivityToString(sensitivity));
    }

    Request *InputCard::commandToSetE1Bit(int index, Vch003::E1Bit bit)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setE1bit(bit);
        return new vch003request::SetCardParameter(slot(), "ssmB" + QString::number(index + 1), vch003request::e1bitToString(bit));
    }

    Request *InputCard::commandToSetMeasurementMode(int index, bool measurementMode)
    {
        if(index < 0 || index >= inputChannels_.size())
            return nullptr;
        if(SSU::kChangedParamImmediately)
            static_cast<InChannelInputCard*>(inputChannels_[index])->setMeasurementMode(measurementMode);
        return new vch003request::SetCardParameter(slot(), "meaM" + QString::number(index + 1), measurementMode ? "ON" : "OFF");
    }
}
