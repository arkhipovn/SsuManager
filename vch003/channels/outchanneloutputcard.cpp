#include "outchanneloutputcard.h"

namespace ssu {
    OutChannelOutputCard::OutChannelOutputCard(QObject *parent) : Channel(parent),
        type_(SSU::SignalType::Off),
        state_(true),
        holdoverModeEnabled_(true)
    {

    }

    SSU::SignalType OutChannelOutputCard::type() const
    {
        return type_;
    }

    void OutChannelOutputCard::setType(SSU::SignalType type)
    {
        if(type_ != type) {
            type_ = type;
            emit typeChanged();
        }
    }
    
    bool OutChannelOutputCard::state() const
    {
        return state_;
    }
    
    void OutChannelOutputCard::setState(bool value)
    {
        if(state_ != value) {
            state_ = value;
            emit stateChanged();
        }
    }
    
    bool OutChannelOutputCard::holdoverModeEnabled() const
    {
        return holdoverModeEnabled_;
    }
    
    void OutChannelOutputCard::setHoldoverModeEnabled(bool value)
    {
        if(holdoverModeEnabled_ != value) {
            holdoverModeEnabled_ = value;
            emit holdoverModeEnabledChanged();
        }
    }
    
}
