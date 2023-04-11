#ifndef NETWORKCONFIGURATION_H
#define NETWORKCONFIGURATION_H

#include <QObject>
#include "ptp.h"
#include "ntp.h"

namespace ssu {

    class NetworkConfiguration : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)

        Q_PROPERTY(QString ip4 READ ip4 NOTIFY ip4Changed)
        Q_PROPERTY(QString mask READ mask NOTIFY maskChanged)
        Q_PROPERTY(QString gateway READ gateway NOTIFY gatewayChanged)

        Q_PROPERTY(Ntp *ntp READ ntp CONSTANT)
        Q_PROPERTY(Ptp *ptp READ ptp CONSTANT)

    public:
        explicit NetworkConfiguration(QObject *parent = nullptr);

        QString mask() const;
        void setMask(const QString &mask);

        QString gateway() const;
        void setGateway(const QString &gateway);

        QString ip4() const;
        void setIp4(const QString &ip4);

        Ntp *ntp() const;
        Ptp *ptp() const;

        bool isValid() const;
        void setValid(bool valid);

    signals:
        void validChanged();

        void ip4Changed();
        void maskChanged();
        void gatewayChanged();

    private:
        bool valid_;

        QString ip4_;
        QString mask_;
        QString gateway_;

        Ntp *ntp_;
        Ptp *ptp_;
    };
}

#endif // NETWORKCONFIGURATION_H
