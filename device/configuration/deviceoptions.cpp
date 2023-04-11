#include "deviceoptions.h"

ssu::DeviceOptions::DeviceOptions(int numberOfSlots,
                                  const QList <int> &measIntervals,
                                  const QList <int> &thresholdIntervals,
                                  int maxNumberOfAuthorizationAttempts, QObject *parent) : QObject(parent),
    kMeasIntervals(measIntervals),
    kThresholdIntervals(thresholdIntervals),
    kNumberOfSlots(numberOfSlots),
    kMaxNumberOfAuthorizationAttempts(maxNumberOfAuthorizationAttempts),
    userRights_(nullptr)
{

}

qreal ssu::DeviceOptions::slotWidth(qreal panelWidth) const
{
    return (panelWidth - kNumberOfSlots + 1) / kNumberOfSlots;
}

//int SSU::checkPassword(const QString &password)
//{
//    int rating = 0;
//    if(password.size() > 6) ++rating;
//    if(password.size() > 10) ++rating;
//    if(password.contains(QRegExp("\\d"))) ++rating;
//    if(password.contains(QRegExp("[A-Z]"))) ++rating;
//    if(password.contains(QRegExp("[a-z]"))) ++rating;
//    if(password.contains(QRegExp("[!@#$%()&\\._,:;]"))) ++rating;
//    return rating;
//}

bool ssu::DeviceOptions::checkPassword(const QString &password)
{
    if(password.size() < 8) return false;
    if(!password.contains(QRegExp("\\d"))) return false;
    if(!password.contains(QRegExp("[A-Z]"))) return false;
    if(!password.contains(QRegExp("[a-z]"))) return false;
    if(!password.contains(QRegExp("[!?-*~+@#$%()&\\._,:;]"))) return false;
    return true;
}

ssu::UserRights *ssu::DeviceOptions::userRights() const
{
    return userRights_;
}
