#ifndef INCHANNELOUTPUTCARD_H
#define INCHANNELOUTPUTCARD_H

#include "device/panel/channel.h"

namespace ssu {
    class InChannelOutputCard : public Channel
    {
        Q_OBJECT

        Q_PROPERTY(bool ban READ ban NOTIFY banChanged)
        Q_PROPERTY(bool home READ home NOTIFY homeChanged)
        Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
        Q_PROPERTY(int currentPriority READ currentPriority NOTIFY currentPriorityChanged)

    public:
        explicit InChannelOutputCard(QObject *parent = nullptr);

        bool ban() const;
        void setBan(bool ban);

        int priority() const;
        void setPriority(int priority);

        int currentPriority() const;
        void setCurrentPriority(int currentPriority);

        bool home() const;
        void setHome(bool home);

    signals:
        void banChanged();
        void priorityChanged();
        void currentPriorityChanged();
        void homeChanged();

    private:
        bool ban_;          // состояние шины БАН с ПЛИСосн
        bool home_;         // состояние шины НОМЕ с ПЛИСосн. 0 - отсутствие сигнала
        int priority_;
        int currentPriority_;
    };
}


#endif // INCHANNELOUTPUTCARD_H
