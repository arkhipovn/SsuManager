#include "csettings.h"
#include <QSettings>
#include "CTranslator"
#include "src/QCC/qccutils.h"
#include "ssu.h"
#include "global.h"

namespace ssu {

    CSettings *CSettings::getInstance()
    {
        static CSettings settings;
        return &settings;
    }

    QString CSettings::dateTimeFormat() const
    {
        return dateFormat_ + " " + timeFormat_;
    }

    CSettings::CSettings(QObject *parent) : QObject(parent),
        timeSpec_(Qt::LocalTime),
        timeFormat_("HH:mm:ss"),
        dateFormat_("dd.MM.yyyy"),
        pollingInterval_(10000),
        measDataPollInterval_(60000),
        measLogPollInterval_(-3600000),
        userLogPollInterval_(-3600000),
        maximumTimeDifference_(240000),
        updatingCardParametersOnEventEnabled_(true),
        updatingLogsOnEventEnabled_(false),
        displayingValuesWithoutCoefficients_(false)
    {
        alarmColorIndications_ = {
            { static_cast<int>(SSU::EventStatus::Critical), QColor("#FF9FA2") },
            { static_cast<int>(SSU::EventStatus::Major),    QColor("#FFAD9F") },
            { static_cast<int>(SSU::EventStatus::Minor),    QColor("#FFBF9F") },
            { static_cast<int>(SSU::EventStatus::Warning),  QColor("#FFE59E") },
            { static_cast<int>(SSU::EventStatus::NotAlarm), QColor("#9EFFFF") }
        };

        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.setIniCodec("UTF-8");

        s.beginGroup(GENERAL_SETTINGS_GROUP);
        timeSpec_ = static_cast<Qt::TimeSpec>(s.value("TimeSpec", timeSpec_).toInt());
        timeFormat_ = s.value("PTimeFormat", timeFormat_).toString();
        dateFormat_ = s.value("PDateFormat", dateFormat_).toString();

        pollingInterval_ = s.value("PollingInterval", pollingInterval_).toInt();
        measDataPollInterval_ = s.value("MeasDataPollInterval", measDataPollInterval_).toInt();
        measLogPollInterval_ = s.value("MeasLogPollInterval", measLogPollInterval_).toInt();
        userLogPollInterval_ = s.value("UserLogPollInterval", userLogPollInterval_).toInt();
        maximumTimeDifference_ = s.value("MaximumTimeDifference", maximumTimeDifference_).toInt();
        displayingValuesWithoutCoefficients_ = s.value("DisplayingValuesWithoutCoefficients", displayingValuesWithoutCoefficients_).toBool();

        s.endGroup();

        s.beginGroup(EVENTS_SETTINGS_GROUP);
        updatingCardParametersOnEventEnabled_ = s.value("UpdatingCardParametersOnEventEnabled", updatingCardParametersOnEventEnabled_).toBool();
        updatingLogsOnEventEnabled_ = s.value("UpdatingLogsOnEventEnabled", updatingLogsOnEventEnabled_).toBool();
        s.endGroup();


        s.beginGroup(ALARM_COLOR_INDICATIONS_SETTINGS_GROUP);
        auto colorKeys = s.childKeys();
        bool isok;
        for(const QString &colorKey : colorKeys) {
            auto status = colorKey.toInt(&isok);
            if(!isok)
                continue;
            auto color = s.value(colorKey, "").toString();
            if(color.isEmpty()) continue;

            auto it = alarmColorIndications_.find(status);
            if(it == alarmColorIndications_.end()) {
                alarmColorIndications_.insert(status, color);
            }
            else {
                it.value() = color;
            }
        }
        s.endGroup();
    }
    
    bool CSettings::displayingValuesWithoutCoefficients() const
    {
        return displayingValuesWithoutCoefficients_;
    }

    QColor CSettings::alarmColorIndication(SSU::EventStatus status) const
    {
        auto it = alarmColorIndications_.constFind(static_cast<int>(status));
        return it == alarmColorIndications_.constEnd() ? Qt::white
                                                       : it.value();
    }

    void CSettings::setAlarmColorIndication(SSU::EventStatus status, const QColor &color)
    {
        auto it = alarmColorIndications_.find(static_cast<int>(status));
        if(it == alarmColorIndications_.end()) {
            alarmColorIndications_.insert(static_cast<int>(status), color);
        }
        else {
            it.value() = color;
        }
        set(ALARM_COLOR_INDICATIONS_SETTINGS_GROUP, QString::number(static_cast<int>(status)), color.name());
        emit alarmColorIndicationsChanged();
    }
    
    int CSettings::measLogPollInterval() const
    {
        return measLogPollInterval_;
    }
    
    void CSettings::setMeasLogPollInterval(int measLogPollInterval)
    {
        if(measLogPollInterval_ != measLogPollInterval) {
            measLogPollInterval_ = measLogPollInterval;
            set(GENERAL_SETTINGS_GROUP, "MeasLogPollInterval", measLogPollInterval);
            emit measLogPollIntervalChanged(measLogPollInterval);
        }
    }

    QDateTime CSettings::dateTimeFromString(const QString &time) const
    {
        if(time.isEmpty()) return QDateTime();
        QDateTime datetime = QDateTime::fromString(time, dateTimeFormat());
        if(!datetime.isValid())
            return QDateTime();
        return QCCUtils::dateTimeFromQml(datetime, timeSpec_);
    }

    QDateTime CSettings::getCurrentDateTime(int addSec) const
    {
        auto now = QDateTime::currentDateTimeUtc().addSecs(addSec);
        return QCCUtils::qmlDateTime(now, timeSpec_);
    }

    bool CSettings::updatingCardParametersOnEventEnabled() const
    {
        return updatingCardParametersOnEventEnabled_;
    }

    void CSettings::setUpdatingCardParametersOnEventEnabled(bool value)
    {
        if(updatingCardParametersOnEventEnabled_ != value) {
            updatingCardParametersOnEventEnabled_ = value;
            set(EVENTS_SETTINGS_GROUP, "UpdatingCardParametersOnEventEnabled", value);
            emit updatingOnEventChanged();
        }
    }

    bool CSettings::updatingLogsOnEventEnabled() const
    {
        return updatingLogsOnEventEnabled_;
    }

    void CSettings::setUpdatingLogsOnEventEnabled(bool value)
    {
        if(updatingLogsOnEventEnabled_ != value) {
            updatingLogsOnEventEnabled_ = value;
            set(EVENTS_SETTINGS_GROUP, "UpdatingLogsOnEventEnabled", updatingLogsOnEventEnabled_);
            emit updatingOnEventChanged();
        }
    }

    void CSettings::setUpdatingOnEventEnabled(bool updatingCardParametersOnEventEnabled, bool updatingLogsOnEventEnabled)
    {
        if(updatingCardParametersOnEventEnabled_ != updatingCardParametersOnEventEnabled ||
                updatingLogsOnEventEnabled_ != updatingLogsOnEventEnabled) {
            updatingCardParametersOnEventEnabled_ = updatingCardParametersOnEventEnabled;
            updatingLogsOnEventEnabled_ = updatingLogsOnEventEnabled;
            set(EVENTS_SETTINGS_GROUP, "UpdatingCardParametersOnEventEnabled", updatingCardParametersOnEventEnabled);
            set(EVENTS_SETTINGS_GROUP, "UpdatingLogsOnEventEnabled", updatingLogsOnEventEnabled);
            emit updatingOnEventChanged();
        }
    }

    int CSettings::maximumTimeDifference() const
    {
        return maximumTimeDifference_;
    }

    void CSettings::setMaximumTimeDifference(int maximumTimeDifference)
    {
        if(maximumTimeDifference_ != maximumTimeDifference) {
            maximumTimeDifference_ = maximumTimeDifference;
            set(GENERAL_SETTINGS_GROUP, "MaximumTimeDifference", maximumTimeDifference);
            emit maximumTimeDifferenceChanged(maximumTimeDifference);
        }
    }

    QVariant CSettings::get(const QString &group, const QString &key, const QVariant &defaultValue)
    {
        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.beginGroup(group);
        auto v = s.value(key, defaultValue);
        s.endGroup();
        return v;
    }

    void CSettings::set(const QString &group, const QString &key, const QVariant &value)
    {
        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.setIniCodec("UTF-8");
        s.beginGroup(group);
        s.setValue(key, value);
        s.endGroup();
    }

    void CSettings::clear(const QString &group)
    {
        QSettings s(SSU::programSettingsFile(), QSettings::IniFormat);
        s.beginGroup(group);
        s.clear();
        s.endGroup();
    }

    void CSettings::setCurrentLanguage(QLocale::Language language)
    {
        set(GENERAL_SETTINGS_GROUP, "Language", language);
    }

    int CSettings::userLogPollInterval() const
    {
        return userLogPollInterval_;
    }

    void CSettings::setUserLogPollInterval(int userLogPollInterval)
    {
        if(userLogPollInterval_ != userLogPollInterval) {
            userLogPollInterval_ = userLogPollInterval;
            set(GENERAL_SETTINGS_GROUP, "UserLogPollInterval", userLogPollInterval);
            emit userLogPollIntervalChanged(userLogPollInterval);
        }
    }

    int CSettings::measDataPollInterval() const
    {
        return measDataPollInterval_;
    }

    void CSettings::setMeasDataPollInterval(int measDataPollInterval)
    {
        if(measDataPollInterval_ != measDataPollInterval) {
            measDataPollInterval_ = measDataPollInterval;
            set(GENERAL_SETTINGS_GROUP, "MeasDataPollInterval", measDataPollInterval);
            emit measDataPollIntervalChanged(measDataPollInterval);
        }
    }

    int CSettings::pollingInterval() const
    {
        return pollingInterval_;
    }

    void CSettings::setPollingInterval(int pollingInterval)
    {
        if(pollingInterval_ != pollingInterval) {
            pollingInterval_ = pollingInterval;
            set(GENERAL_SETTINGS_GROUP, "PollingInterval", pollingInterval);
            emit pollingIntervalChanged(pollingInterval);
        }
    }

    QString CSettings::dateFormat() const
    {
        return dateFormat_;
    }

    void CSettings::setDateFormat(const QString &dateFormat)
    {
        if(dateFormat_ != dateFormat) {
            dateFormat_ = dateFormat;
            set(GENERAL_SETTINGS_GROUP, "PDateFormat", dateFormat);
            emit dateTimeFormatChanged(dateTimeFormat());
        }
    }

    QString CSettings::timeFormat() const
    {
        return timeFormat_;
    }

    void CSettings::setTimeFormat(const QString &timeFormat)
    {
        if(timeFormat_ != timeFormat) {
            timeFormat_ = timeFormat;
            set(GENERAL_SETTINGS_GROUP, "PTimeFormat", timeFormat);
            emit dateTimeFormatChanged(dateTimeFormat());
        }
    }

    Qt::TimeSpec CSettings::timeSpec() const
    {
        return timeSpec_;
    }

    void CSettings::setTimeSpec(const Qt::TimeSpec &timeSpec)
    {
        if(timeSpec_ != timeSpec) {
            timeSpec_ = timeSpec;
            set(GENERAL_SETTINGS_GROUP, "TimeSpec", timeSpec);
            emit timeSpecChanged(timeSpec);
        }
    }
}
