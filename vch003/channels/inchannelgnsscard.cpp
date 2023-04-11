#include "inchannelgnsscard.h"

namespace ssu {
    InChannelGnssCard::InChannelGnssCard(QObject *parent) : Channel(parent),
        priority_(0),
        waitToRestoreTime_(1),
        state_(true),
        configuration_(true)
    {

    }

    int InChannelGnssCard::priority() const
    {
        return priority_;
    }

    void InChannelGnssCard::setPriority(int priority)
    {
        if(priority_ != priority) {
            priority_ = priority;
            emit priorityChanged();
        }
    }

    int InChannelGnssCard::waitToRestoreTime() const
    {
        return waitToRestoreTime_;
    }

    void InChannelGnssCard::setWaitToRestoreTime(int waitToRestoreTime)
    {
        if(waitToRestoreTime_ != waitToRestoreTime) {
            waitToRestoreTime_ = waitToRestoreTime;
            emit waitToRestoreTimeChanged();
        }
    }

    bool InChannelGnssCard::state() const
    {
        return state_;
    }

    void InChannelGnssCard::setState(bool state)
    {
        if(state_ != state) {
            state_ = state;
            emit stateChanged();
        }
    }

    bool InChannelGnssCard::configuration() const
    {
        return configuration_;
    }

    void InChannelGnssCard::setConfiguration(bool configuration)
    {
        if(configuration_ != configuration) {
            configuration_ = configuration;
            emit configurationChanged();
        }
    }

}
