#ifndef NTP_H
#define NTP_H

#include <QObject>

namespace ssu {
    class Ntp : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool isValid READ isValid NOTIFY validChanged)
        Q_PROPERTY(int stratum READ stratum NOTIFY stratumChanged)

        Q_PROPERTY(QString serverRefId READ serverRefId NOTIFY serverRefIdChanged)

    public:
        explicit Ntp(QObject *parent = nullptr);

        int stratum() const;
        void setStratum(int stratum);

        bool isValid() const;
        void setValid(bool valid);

        QString serverRefId() const;
        void setServerRefId(const QString &serverRefId);

    signals:
        void validChanged();
        void stratumChanged();
        void serverRefIdChanged();

    private:
        bool valid_;
        int stratum_;
        QString serverRefId_;
    };
}

#endif // NTP_H
