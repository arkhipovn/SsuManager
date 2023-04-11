#include "led.h"
#include "ssu.h"

namespace ssu {
    Led::Led(QObject *parent) : QObject(parent),
        color_(Qt::green),
        power_(SSU::kLedTesting),
        blink_(false),
        blinkInterval_(1000)
    {

    }

    Led::Led(const QColor &color, QObject *parent) : QObject(parent),
        color_(color),
        power_(SSU::kLedTesting),
        blink_(false)
    {

    }

    void Led::setState(const LedState &state)
    {
        setBlinkInterval(state.blinkInterval);
        setState(state.power, state.color, state.blink);
    }

    void Led::setState(bool power, const QColor &color, bool blink)
    {
        if(power_ != power) {
            if(power) {
                // появилось питание..
                // поменяем состояния
                setColor(color);
                setBlink(blink);
                // только потом вкл
                power_ = power;
            }
            else {
                // пропало.. Сделаем все в обратном порядке
                power_ = power;
                setColor(color);
                setBlink(blink);
            }
            emit powerChanged();
        }
        else {
            setColor(color);
            setBlink(blink);
        }
    }

    void Led::turnOn()
    {
        setPower(true);
    }

    void Led::turnOff()
    {
        return setPower(false);
    }

    bool Led::power() const
    {
        return power_;
    }

    void Led::setPower(bool power)
    {
        if(power_ != power) {
            power_ = power;
            emit powerChanged();
        }
    }

    bool Led::blink() const
    {
        return blink_;
    }

    void Led::setBlink(bool blink)
    {
        if(blink_ != blink) {
            blink_ = blink;
            emit blinkChanged();
        }
    }

    QColor Led::color() const
    {
        return color_;
    }

    void Led::setColor(const QColor &color)
    {
        if(color_ != color) {
            color_ = color;
            emit colorChanged();
        }
    }

    int Led::blinkInterval() const
    {
        return blinkInterval_;
    }

    void Led::setBlinkInterval(int blinkInterval)
    {
        if(blinkInterval_ != blinkInterval) {
            blinkInterval_ = blinkInterval;
            emit blinkIntervalChanged();
        }
    }
}
