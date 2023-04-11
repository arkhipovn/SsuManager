#ifndef OUTCHANNELOUTPUTCARD_H
#define OUTCHANNELOUTPUTCARD_H

#include <QObject>

#include "device/panel/channel.h"

namespace ssu {
    class   OutChannelOutputCard : public Channel
    {
        Q_OBJECT

        Q_PROPERTY(SSU::SignalType type READ type NOTIFY typeChanged)
        Q_PROPERTY(bool state READ state NOTIFY stateChanged)
        Q_PROPERTY(bool holdoverModeEnabled READ holdoverModeEnabled NOTIFY holdoverModeEnabledChanged)

    public:
        explicit OutChannelOutputCard(QObject *parent = nullptr);

        SSU::SignalType type() const;
        void setType(SSU::SignalType type);

        bool state() const;
        void setState(bool value);

        bool holdoverModeEnabled() const;
        void setHoldoverModeEnabled(bool value);

    signals:
        void typeChanged();
        void stateChanged();
        void holdoverModeEnabledChanged();

    private:
        SSU::SignalType type_;
        bool state_;                // Флаг состояния выходов
        bool holdoverModeEnabled_;  // Флаг разрешения выходов в режиме Holdover (false - запрет, true – разрешение)
    };
}


#endif // OUTCHANNELOUTPUTCARD_H
