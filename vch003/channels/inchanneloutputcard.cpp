#include "inchanneloutputcard.h"


namespace ssu {
    InChannelOutputCard::InChannelOutputCard(QObject *parent) : Channel(parent),
        ban_(false),
        home_(false),
        priority_(0),
        currentPriority_(0)
    {

    }

    bool InChannelOutputCard::ban() const
    {
        return ban_;
    }

    void InChannelOutputCard::setBan(bool ban)
    {
        if(ban_ != ban) {
            ban_ = ban;
            emit banChanged();
        }
    }

    int InChannelOutputCard::priority() const
    {
        return priority_;
    }

    void InChannelOutputCard::setPriority(int priority)
    {
        if(priority_ != priority) {
            priority_ = priority;
            emit priorityChanged();
        }
    }

    int InChannelOutputCard::currentPriority() const
    {
        return currentPriority_;
    }

    void InChannelOutputCard::setCurrentPriority(int currentPriority)
    {
        if(currentPriority_ != currentPriority) {
            currentPriority_ = currentPriority;
            emit currentPriorityChanged();
        }
    }

    bool InChannelOutputCard::home() const
    {
        return home_;
    }

    void InChannelOutputCard::setHome(bool home)
    {
        if(home_ != home) {
            home_ = home;
            emit homeChanged();
        }
    }

}
