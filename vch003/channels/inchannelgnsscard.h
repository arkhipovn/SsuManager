#ifndef INCHANNELGNSSCARD_H
#define INCHANNELGNSSCARD_H

#include "device/panel/channel.h"

namespace ssu {
    class InChannelGnssCard : public Channel
    {
        Q_OBJECT

        Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
        Q_PROPERTY(int waitToRestoreTime READ waitToRestoreTime NOTIFY waitToRestoreTimeChanged)
        Q_PROPERTY(bool state READ state NOTIFY stateChanged)
        Q_PROPERTY(bool configuration READ configuration NOTIFY configurationChanged)

    public:
        explicit InChannelGnssCard(QObject *parent = nullptr);

        int priority() const;
        void setPriority(int priority);

        int waitToRestoreTime() const;
        void setWaitToRestoreTime(int waitToRestoreTime);

        bool state() const;
        void setState(bool state);

        bool configuration() const;
        void setConfiguration(bool configuration);

    signals:
        void priorityChanged();
        void waitToRestoreTimeChanged();
        void stateChanged();
        void configurationChanged();

    private:
        int priority_;
        int waitToRestoreTime_;
        bool state_;                // статус входов. Есть/Нет
        bool configuration_;        // конфигурация входов. Вкл/выкл
    };

}

#endif // INCHANNELGNSSCARD_H
