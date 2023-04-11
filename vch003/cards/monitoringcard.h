#ifndef MONITORINGCARD_H
#define MONITORINGCARD_H

#include "powercard.h"
#include "vch003card.h"
#include "vch003/vch003.h"

namespace ssu {
    class MonitoringCard : public Vch003Card
    {
        Q_OBJECT

        Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)

        Q_PROPERTY(PowerCard *powerA READ powerA CONSTANT)
        Q_PROPERTY(PowerCard *powerB READ powerB CONSTANT)

        Q_PROPERTY(QString ip READ ip NOTIFY ipChanged)
        Q_PROPERTY(QString mask READ mask NOTIFY maskChanged)
        Q_PROPERTY(quint16 port READ port NOTIFY portChanged)

        Q_PROPERTY(bool thresholdsEnabled READ thresholdsEnabled NOTIFY thresholdsEnabledChanged)

    public:
        explicit MonitoringCard(int address = -1, QObject *parent = nullptr);

        void parseParameters(const QString &parameterList) override;

        QString deviceName() const;
        void setDeviceName(const QString &deviceName);

        QString ip() const;
        void setIp(const QString &ip);

        QString mask() const;
        void setMask(const QString &mask);

        quint16 port() const;
        void setPort(const quint16 &port);

        bool thresholdsEnabled() const;
        void setThresholdsEnabled(bool thresholdsEnabled);

        PowerCard *powerA() const;
        PowerCard *powerB() const;

        // Команды управления
        Q_INVOKABLE Request *commandToSetDeviceName(const QString &name);                               // установка имени устройства
        Q_INVOKABLE Request *commandToSetDateTime(const QDateTime &time);                               // установка даты устройства
        Q_INVOKABLE Request *synchronizeTimeWithComputer();                                             // установка даты устройства
        Q_INVOKABLE Request *commandToSetNetworkSettings(const QString &address,
                                                         quint64 port,
                                                         const QString &mask = "");
        Q_INVOKABLE Request *enableThresholdCommand(bool enable);                                      // команда отключения пороговых значений

    signals:
        void deviceNameChanged();
        void maskChanged();
        void ipChanged();
        void portChanged();
        void thresholdsEnabledChanged();

    private:
        QString deviceName_;

        QString ip_;
        QString mask_;
        quint16 port_;

        PowerCard *powerA_;
        PowerCard *powerB_;

        bool thresholdsEnabled_;
    };
}

#endif // MONITORINGCARD_H
