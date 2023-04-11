#include "lanconnection.h"

ssu::LanConnection::LanConnection(QObject *parent) : ConnectionSettings(parent),
    port_(0)
{

}

ssu::LanConnection::LanConnection(const QString &hostname, quint16 port, QObject *parent) : ConnectionSettings(parent),
    hostname_(hostname),
    port_(port)
{

}

void ssu::LanConnection::setConnectionParameters(const QString &hostname, quint16 port)
{
    bool changed = false;
    if(hostname_ != hostname) {
        hostname_ = hostname;
        // если поменяли хост, то очистим всю информацию о пользователе
        clearAuthorizationSettings();
        changed = true;
    }
    if(port_ != port) {
        port_ = port;
        changed = true;
    }
    if(changed)
        emit connectionParametersChanged();
}

QString ssu::LanConnection::hostname() const
{
    return hostname_;
}

quint16 ssu::LanConnection::port() const
{
    return port_;
}

QString ssu::LanConnection::name() const
{
    return hostname_;
}

