#include "ntp.h"

namespace ssu {
    Ntp::Ntp(QObject *parent) : QObject(parent),
        valid_(false),
        stratum_(0)
    {

    }

    int Ntp::stratum() const
    {
        return stratum_;
    }

    void Ntp::setStratum(int stratum)
    {
        if(stratum_ != stratum) {
            stratum_ = stratum;
            emit stratumChanged();
        }
    }

    bool Ntp::isValid() const
    {
        return valid_;
    }

    void Ntp::setValid(bool valid)
    {
        if(valid_ != valid) {
            valid_ = valid;
            emit validChanged();
        }
    }

    QString Ntp::serverRefId() const
    {
        return serverRefId_;
    }

    void Ntp::setServerRefId(const QString &serverRefId)
    {
        if(serverRefId_ != serverRefId) {
            serverRefId_ = serverRefId;
            emit serverRefIdChanged();
        }
    }
}
