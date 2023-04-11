#include "deviceresponsehandler.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "vch003/monitorring/vch003requests.h"

ssu::DeviceResponseHandler::DeviceResponseHandler(ssu::Device *device) : ResponseHandler(),
    device_(device)
{

}

bool ssu::DeviceResponseHandler::parseData(const QString &cmd, const QString &data, QString &slot, QString &command, QString &request) const
{
    //    QRegularExpression re("((A\\d{2}) " + cmd + ".*|" + cmd + ".*)\r\n(" + REGEXP_TIME + "), [^,]*, [^\\s,]*");
    QRegularExpression re("(((A\\d{2}) )?" + cmd + "( .*)?)\\r\\n(" + REGEXP_TIME + "), [^,]*, [^\\s,]*");
    QRegularExpressionMatch match = re.match(data);

    if(match.hasMatch()) {
        slot = match.captured(3);
        command = match.captured(1);
        request = data.mid(match.capturedEnd());

        QDateTime time = QDateTime::fromString(match.captured(5), DEVICE_TIME_FORMAT);
        if(time.isValid()) {
            time.setTimeSpec(Qt::UTC);
            if(device_ != nullptr)
                device_->setCurrentDateTime(time);
        }
        return true;
    }
    return false;
}


