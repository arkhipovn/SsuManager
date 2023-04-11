#include "inchannelinputcard.h"

using namespace ssu;

InChannelInputCard::InChannelInputCard(QObject *parent) : Channel(parent),
    ban_(false),
    state_(0),
    type_(SSU::SignalType::Off),
    statusThreshold_(SSU::StatusThreshold::None),
    ssm_(SSU::SSM::None),
    waitToRestoreTime_(1),
    measurementMode_(false),
    e1bit_(Vch003::E1Bit::Sa4),
    failureTransmEndSensitivity_(Vch003::Sensitivity::Off),
    crcErrorsSensitivity_(Vch003::Sensitivity::Off),
    lossOfMultiFrameSensitivity_(Vch003::Sensitivity::Off),
    lossOfFrameSensitivity_(Vch003::Sensitivity::Off),
    hdb3ErrorSensitivity_(Vch003::Sensitivity::Sensitivity0)
{

}

bool InChannelInputCard::ban() const
{
    return ban_;
}

void InChannelInputCard::setBan(bool ban)
{
    if(ban_ != ban) {
        ban_ = ban;
        emit banChanged();
    }
}

int InChannelInputCard::state() const
{
    return state_;
}

void InChannelInputCard::setState(int state)
{
    if(state_ != state) {
        state_ = state;
        emit stateChanged();
    }
}

SSU::SignalType InChannelInputCard::type() const
{
    return type_;
}

void InChannelInputCard::setType(const SSU::SignalType &type)
{
    if(type_ != type) {
        type_ = type;
        emit typeChanged();
    }
}

SSU::StatusThreshold InChannelInputCard::statusThreshold() const
{
    return statusThreshold_;
}

void InChannelInputCard::setStatusThreshold(SSU::StatusThreshold statusThreshold)
{
    if(statusThreshold_ != statusThreshold) {
        statusThreshold_ = statusThreshold;
        emit statusThresholdChanged();
    }
}

SSU::SSM InChannelInputCard::ssm() const
{
    return ssm_;
}

void InChannelInputCard::setSsm(SSU::SSM ssm)
{
    if(ssm_ != ssm) {
        ssm_ = ssm;
        emit ssmChanged();
    }
}

int InChannelInputCard::waitToRestoreTime() const
{
    return waitToRestoreTime_;
}

void InChannelInputCard::setWaitToRestoreTime(int waitToRestoreTime)
{
    if(waitToRestoreTime_ != waitToRestoreTime) {
        waitToRestoreTime_ = waitToRestoreTime;
        emit waitToRestoreTimeChanged();
    }
}

bool InChannelInputCard::measurementMode() const
{
    return measurementMode_;
}

void InChannelInputCard::setMeasurementMode(bool measurementMode)
{
    if(measurementMode_ != measurementMode) {
        measurementMode_ = measurementMode;
        emit measurementModeChanged();
    }
}

Vch003::E1Bit InChannelInputCard::e1bit() const
{
    return e1bit_;
}

void InChannelInputCard::setE1bit(Vch003::E1Bit e1bit)
{
    if(e1bit_ != e1bit) {
        e1bit_ = e1bit;
        emit e1bitChanged();
    }
}

Vch003::Sensitivity InChannelInputCard::failureTransmEndSensitivity() const
{
    return failureTransmEndSensitivity_;
}

void InChannelInputCard::setFailureTransmEndSensitivity(Vch003::Sensitivity failureTransmEndSensitivity)
{
    if(failureTransmEndSensitivity_ != failureTransmEndSensitivity) {
        failureTransmEndSensitivity_ = failureTransmEndSensitivity;
        emit failureTransmEndSensitivityChanged();
    }
}

Vch003::Sensitivity InChannelInputCard::crcErrorsSensitivity() const
{
    return crcErrorsSensitivity_;
}

void InChannelInputCard::setCrcErrorsSensitivity(Vch003::Sensitivity crcErrorsSensitivity)
{
    if(crcErrorsSensitivity_ != crcErrorsSensitivity) {
        crcErrorsSensitivity_ = crcErrorsSensitivity;
        emit crcErrorsSensitivityChanged();
    }
}

Vch003::Sensitivity InChannelInputCard::lossOfMultiFrameSensitivity() const
{
    return lossOfMultiFrameSensitivity_;
}

void InChannelInputCard::setLossOfMultiFrameSensitivity(Vch003::Sensitivity lossOfMultiFrameSensitivity)
{
    if(lossOfMultiFrameSensitivity_ != lossOfMultiFrameSensitivity) {
        lossOfMultiFrameSensitivity_ = lossOfMultiFrameSensitivity;
        emit lossOfMultiFrameSensitivityChanged();
    }
}

Vch003::Sensitivity InChannelInputCard::lossOfFrameSensitivity() const
{
    return lossOfFrameSensitivity_;
}

void InChannelInputCard::setLossOfFrameSensitivity(Vch003::Sensitivity lossOfFrameSensitivity)
{
    if(lossOfFrameSensitivity_ != lossOfFrameSensitivity) {
        lossOfFrameSensitivity_ = lossOfFrameSensitivity;
        emit lossOfFrameSensitivityChanged();
    }
}

Vch003::Sensitivity InChannelInputCard::hdb3ErrorSensitivity() const
{
    return hdb3ErrorSensitivity_;
}

void InChannelInputCard::setHdb3ErrorSensitivity(Vch003::Sensitivity hdb3ErrorSensitivity)
{
    if(hdb3ErrorSensitivity_ != hdb3ErrorSensitivity) {
        hdb3ErrorSensitivity_ = hdb3ErrorSensitivity;
        emit hdb3ErrorSensitivityChanged();
    }
}
