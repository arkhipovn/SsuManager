#ifndef CARD_H
#define CARD_H

#include <QObject>
#include "device/led/ledlist.h"
#include "channel.h"

#include <QDebug>

namespace ssu {
    class Card : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(int type READ type CONSTANT)
        Q_PROPERTY(int address READ address CONSTANT)
        Q_PROPERTY(QString slot READ slot CONSTANT)

        Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
        Q_PROPERTY(QString software READ software NOTIFY softwareChanged)
        Q_PROPERTY(QString hardware READ hardware NOTIFY hardwareChanged)

        Q_PROPERTY(LedList *leds READ leds CONSTANT)

        Q_PROPERTY(QQmlListProperty <QObject> inputChannels READ inputChannels CONSTANT)    //Channel
        Q_PROPERTY(QQmlListProperty <QObject> outputChannels READ outputChannels CONSTANT)    //Channel

    public:
        explicit Card(int type = 0, int address = -1, QObject *parent = nullptr);
        virtual ~Card();

        QQmlListProperty <Channel> inputChannels();
        QQmlListProperty <Channel> outputChannels();

        LedList *leds() const;

        int type() const;
        int address() const;

        QString serialNumber() const;
        void setSerialNumber(const QString &serialNumber);

        QString software() const;
        void setSoftware(const QString &software);

        QString hardware() const;
        void setHardware(const QString &hardware);

        QString slot() const;

        bool operator ==(const Card &card) const;
        bool operator <(Card &card);

        Q_INVOKABLE Channel *inputChannelAt(int index) const;
        Q_INVOKABLE Channel *outputChannelAt(int index) const;

    signals:
        void serialNumberChanged();
        void softwareChanged();
        void hardwareChanged();

    protected:
        const int type_;              // тип платы
        const int address_;

        QString serialNumber_;
        QString software_;
        QString hardware_;

        LedList *leds_;

        // Входные и выходные каналы у платы
        QList <Channel*> inputChannels_;
        QList <Channel*> outputChannels_;

        static int numberOfChannels(QQmlListProperty <Channel> *list);
        static Channel* atChannel(QQmlListProperty <Channel> *list, int index);
    };

    struct CardInfo
    {
        int type;
        int address;
        QString serialNumber;
        QString software;
        QString hardware;
    };
}

QDebug operator <<(QDebug stream, const ssu::Card &card);
QDebug operator <<(QDebug stream, const ssu::Card *card);

#endif // CARD_H
