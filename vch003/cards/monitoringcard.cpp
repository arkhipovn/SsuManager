#include "monitoringcard.h"
#include "ssu.h"

#include <QRegularExpression>
#include <QTextStream>

namespace ssu {

    MonitoringCard::MonitoringCard(int address, QObject *parent) : Vch003Card(SSU::Monitoring, address, parent),
        port_(5000),
        thresholdsEnabled_(true)
    {
        powerA_ = new PowerCard(19, this);
        powerB_ = new PowerCard(19, this);

        leds_ = new LedList({
                                Qt::darkGreen,     // Питание
                                Qt::red,           // Несущ.
                                Qt::red,           // Сущ.
                                Qt::red,           // Критич.
                                Qt::yellow,        // Остат.
                                Qt::darkGreen,     // Питание А
                                Qt::darkGreen      // Питание B
                            },
                            this);

        if(SSU::kImitationWork) {
            thresholdsEnabled_ = false;
            deviceName_ = "VCH-003";
            ip_ = "192.168.1.0";
            mask_ = "255.255.255.0";
            port_ = 5005;
        }
    }

    void MonitoringCard::parseParameters(const QString &parameterList)
    {
        //@N переделать..
        QRegularExpression re;
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match;
        bool isok;

        QTextStream stream(const_cast<QString*>(&parameterList), QIODevice::ReadOnly);

        QString line;
        while(stream.readLineInto(&line)) {
            if(line.isEmpty())
                continue;

            re.setPattern("^name, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                setDeviceName(match.captured(1));
                continue;
            }

            re.setPattern("^addr, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                setIp(match.captured(1));
                continue;
            }

            re.setPattern("^mask, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                setMask(match.captured(1));
                continue;
            }

            re.setPattern("^port, (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto value = match.capturedRef(1);
                quint64 p = value.toULongLong(&isok);
                if(isok) setPort(p);
                continue;
            }

            re.setPattern("^enableThrs, (ON|OFF)");
            match = re.match(line);
            if(match.hasMatch()) {
                setThresholdsEnabled(match.capturedRef(1).compare(QString("ON"), Qt::CaseInsensitive) == 0);
                continue;
            }

            re.setPattern("^pwr(\\d+), (true|false)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;

                auto value = match.capturedRef(2);
                bool working = value == "false" ? false : true;

                switch (index) {
                    case 1:
                        powerA_->setSupply(working);
                        break;
                    case 2:
                        powerB_->setSupply(working);
                        break;
                    default: break;
                }

                continue;
            }

            re.setPattern("^pwrBlock(\\d+), (true|false)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;

                auto value = match.capturedRef(2);
                bool active = value == "false" ? false : true;

                switch (index) {
                    case 1:
                        powerA_->setCardActive(active);
                        break;
                    case 2:
                        powerB_->setCardActive(active);
                        break;
                    default: break;
                }

                continue;
            }

            re.setPattern("^fuse(\\d+), (true|false)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;

                auto value = match.capturedRef(2);
                bool fuse = value == "false" ? false : true;

                switch (index) {
                    case 1:
                        powerA_->setFuse(fuse);
                        break;
                    case 2:
                        powerB_->setFuse(fuse);
                        break;
                    default: break;
                }

                continue;
            }

            // LED
            re.setPattern("^led(\\d+), (.+)");
            match = re.match(line);
            if(match.hasMatch()) {
                auto index = match.capturedRef(1).toInt(&isok);
                if(!isok) continue;
                if(index < 0 || index >= leds_->size()) // А тут индекс с 0.. (переделать бы в приборе)
                    continue;
                auto value = match.capturedRef(2);
                if(value.isEmpty()) continue;
                leds_->at(index)->setState(vch003request::ledStateFromString(value));
                continue;
            }
        }
    }

    QString MonitoringCard::deviceName() const
    {
        return deviceName_;
    }

    void MonitoringCard::setDeviceName(const QString &deviceName)
    {
        if(deviceName_ != deviceName) {
            deviceName_ = deviceName;
            emit deviceNameChanged();
        }
    }

    QString MonitoringCard::ip() const
    {
        return ip_;
    }

    void MonitoringCard::setIp(const QString &ip)
    {
        if(ip_ != ip) {
            ip_ = ip;
            emit ipChanged();
        }
    }

    QString MonitoringCard::mask() const
    {
        return mask_;
    }

    void MonitoringCard::setMask(const QString &mask)
    {
        if(mask_ != mask) {
            mask_ = mask;
            emit maskChanged();
        }
    }

    quint16 MonitoringCard::port() const
    {
        return port_;
    }

    void MonitoringCard::setPort(const quint16 &port)
    {
        if(port_ != port) {
            port_ = port;
            emit portChanged();
        }
    }

    PowerCard *MonitoringCard::powerA() const
    {
        return powerA_;
    }

    PowerCard *MonitoringCard::powerB() const
    {
        return powerB_;
    }

    Request *MonitoringCard::commandToSetDeviceName(const QString &name)
    {
        if(SSU::kChangedParamImmediately)
            setDeviceName(name);
        return new vch003request::SetCardParameter(slot(), "name", name);
    }

    Request *MonitoringCard::commandToSetDateTime(const QDateTime &time)
    {
        if(!time.isValid())
            return nullptr;
        return new vch003request::SetDeviceDateTime(time);
    }

    Request *MonitoringCard::synchronizeTimeWithComputer()
    {
        return new vch003request::SetDeviceDateTime;
    }

    Request *MonitoringCard::commandToSetNetworkSettings(const QString &address, quint64 port, const QString &mask)
    {
        if(address.isEmpty())
            return nullptr;
        QString arg = address + " " + QString::number(port);
        if(!mask.isEmpty() && mask != mask_)
            arg.append(" " + mask);
        return new vch003request::SetCardParameter(slot(), "network", arg);
    }

    Request *MonitoringCard::enableThresholdCommand(bool enable)
    {
        if(SSU::kChangedParamImmediately)
            setThresholdsEnabled(enable);
        return new vch003request::SetCardParameter(slot(), "enableThrs", enable ? "ON" : "OFF");
    }

    bool MonitoringCard::thresholdsEnabled() const
    {
        return thresholdsEnabled_;
    }

    void MonitoringCard::setThresholdsEnabled(bool thresholdsEnabled)
    {
        if(thresholdsEnabled_ != thresholdsEnabled) {
            thresholdsEnabled_ = thresholdsEnabled;
            emit thresholdsEnabledChanged();
        }
    }
}
