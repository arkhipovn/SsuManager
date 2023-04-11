#ifndef LED_H
#define LED_H

#include <QObject>
#include <QColor>

namespace ssu {
    struct LedState
    {
        QColor color;
        bool power;
        bool blink;
        int blinkInterval;
    };

    class Led : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
        Q_PROPERTY(bool power READ power NOTIFY powerChanged)
        Q_PROPERTY(bool blink READ blink NOTIFY blinkChanged)
        Q_PROPERTY(int blinkInterval READ blinkInterval NOTIFY blinkIntervalChanged)

    public:
        Led(QObject *parent = nullptr);
        Led(const QColor &color, QObject *parent = nullptr);

        void setState(const LedState &state);
        void setState(bool power, const QColor &color, bool blink = false);

        void turnOn();
        void turnOff();

        bool power() const;
        void setPower(bool power);

        bool blink() const;
        void setBlink(bool blink);

        QColor color() const;
        void setColor(const QColor &color);

        int blinkInterval() const;
        void setBlinkInterval(int blinkInterval);

    signals:
        void powerChanged();
        void colorChanged();
        void blinkChanged();
        void blinkIntervalChanged();

    private:
        QColor color_;
        bool power_;
        bool blink_;
        int blinkInterval_;
    };
}

#endif // LED_H
