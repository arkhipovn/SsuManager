#ifndef LANCONNECTION_H
#define LANCONNECTION_H

#include "connectionsettings.h"

namespace ssu {
    class LanConnection : public ConnectionSettings
    {
        Q_OBJECT

        Q_PROPERTY(QString hostname READ hostname NOTIFY connectionParametersChanged)
        Q_PROPERTY(quint16 port READ port NOTIFY connectionParametersChanged)

    public:
        explicit LanConnection(QObject *parent = nullptr);
        explicit LanConnection(const QString &hostname, quint16 port = 5000, QObject *parent = nullptr);

        void setConnectionParameters(const QString &hostname, quint16 port);

        QString hostname() const;
        quint16 port() const;

        Q_INVOKABLE QString name() const override;

    signals:
        void connectionParametersChanged();

    private:
        QString hostname_;
        quint16 port_;
    };
}

#endif // LANCONNECTION_H
