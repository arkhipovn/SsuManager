#include "inchannelgeneratorcard.h"

namespace ssu {
    InChannelGeneratorCard::InChannelGeneratorCard(QObject *parent) : Channel(parent),
        ban_(false),
        priority_(0),
        currentPriority_(0)
    {

    }

    bool InChannelGeneratorCard::ban() const
    {
        return ban_;
    }

    void InChannelGeneratorCard::setBan(bool ban)
    {
        if(ban_ != ban) {
            ban_ = ban;
            emit banChanged();
        }
    }

    int InChannelGeneratorCard::priority() const
    {
        return priority_;
    }

    void InChannelGeneratorCard::setPriority(int priority)
    {
        if(priority_ != priority) {
            priority_ = priority;
            emit priorityChanged();
        }
    }

    int InChannelGeneratorCard::currentPriority() const
    {
        return currentPriority_;
    }

    void InChannelGeneratorCard::setCurrentPriority(int currentPriority)
    {
        if(currentPriority_ != currentPriority) {
            currentPriority_ = currentPriority;
            emit currentPriorityChanged();
        }
    }
}
