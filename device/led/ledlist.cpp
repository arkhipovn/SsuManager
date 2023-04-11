#include "ledlist.h"

namespace ssu {
    LedList::LedList(int count, QObject *parent) : QObject(parent)
    {
        for(int i = 0; i < count; ++i)
            leds.append(new Led(this));
    }

    LedList::LedList(const QList<QColor> &colors, QObject *parent) : QObject(parent)
    {
        for(auto &color : colors) {
            leds.append(new Led(color, this));
        }
    }

    QQmlListProperty<Led> LedList::data()
    {
        return QQmlListProperty <Led>(static_cast<QObject*>(this), static_cast<void*>(&leds),
                                      &LedList::countData, &LedList::atData);
    }

    Led *LedList::operator[](int i)
    {
        return leds[i];
    }

    const Led *LedList::operator[](int i) const
    {
        return leds[i];
    }

    const Led *LedList::at(int i) const
    {
        return leds[i];
    }

    Led *LedList::at(int i)
    {
        return leds[i];
    }

    int LedList::countData(QQmlListProperty<Led> *list)
    {
        auto l = static_cast <QVector <Led*> *> (list->data);
        return l->size();
    }

    Led *LedList::atData(QQmlListProperty<Led> *list, int index)
    {
        auto l = static_cast <QVector <Led*> *> (list->data);
        return l->at(index);
    }
}
