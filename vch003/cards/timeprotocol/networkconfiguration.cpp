#include "networkconfiguration.h"

namespace ssu {
    NetworkConfiguration::NetworkConfiguration(QObject *parent) : QObject(parent),
        valid_(false)
    {
        ntp_ = new Ntp(this);
        ptp_ = new Ptp(this);
    }

    QString NetworkConfiguration::mask() const
    {
        return mask_;
    }

    void NetworkConfiguration::setMask(const QString &mask)
    {
        if(mask_ != mask) {
            mask_ = mask;
            emit maskChanged();
        }
    }

    QString NetworkConfiguration::gateway() const
    {
        return gateway_;
    }

    void NetworkConfiguration::setGateway(const QString &gateway)
    {
        if(gateway_ != gateway) {
            gateway_ = gateway;
            emit gatewayChanged();
        }
    }

    QString NetworkConfiguration::ip4() const
    {
        return ip4_;
    }

    void NetworkConfiguration::setIp4(const QString &ip4)
    {
        if(ip4_ != ip4) {
            ip4_ = ip4;
            emit ip4Changed();
        }
    }

    Ntp *NetworkConfiguration::ntp() const
    {
        return ntp_;
    }

    Ptp *NetworkConfiguration::ptp() const
    {
        return ptp_;
    }

    bool NetworkConfiguration::isValid() const
    {
        return valid_;
    }

    void NetworkConfiguration::setValid(bool valid)
    {
        if(valid_ != valid) {
            valid_ = valid;
            emit validChanged();
        }
    }
}
