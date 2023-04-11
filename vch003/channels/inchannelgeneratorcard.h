#ifndef INCHANNELGENERATORCARD_H
#define INCHANNELGENERATORCARD_H

#include "device/panel/channel.h"

namespace ssu {
    class InChannelGeneratorCard : public Channel
    {
        Q_OBJECT

        Q_PROPERTY(bool ban READ ban NOTIFY banChanged)
        Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
        Q_PROPERTY(int currentPriority READ currentPriority NOTIFY currentPriorityChanged)

    public:
        explicit InChannelGeneratorCard(QObject *parent = nullptr);

        bool ban() const;
        void setBan(bool ban);

        int priority() const;
        void setPriority(int priority);

        int currentPriority() const;
        void setCurrentPriority(int currentPriority);

    signals:
        void banChanged();
        void priorityChanged();
        void currentPriorityChanged();

    private:
        bool ban_;
        int priority_;
        int currentPriority_;
    };
}

#endif // INCHANNELGENERATORCARD_H
