#ifndef LEDLIST_H
#define LEDLIST_H

#include <QQmlListProperty>
#include <QVector>
#include "led.h"

namespace ssu {
    class LedList : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QQmlListProperty <ssu::Led> data READ data CONSTANT)

    public:
        explicit LedList(int count = 0, QObject *parent = nullptr);
        explicit LedList(const QList <QColor> &colors, QObject *parent = nullptr);

        QQmlListProperty <ssu::Led> data();

        Led *operator[](int i);
        const Led *operator[](int i) const;

        const Led *at(int i) const;
        /*Q_INVOKABLE*/ Led *at(int i);

        Q_INVOKABLE inline int size() const { return leds.size(); }

    private:
        QVector <Led*> leds;

        // QQmlListProperty
        static int countData(QQmlListProperty <ssu::Led> *list);
        static Led* atData(QQmlListProperty <ssu::Led> *list, int index);
    };
}

#endif // LEDLIST_H
