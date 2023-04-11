#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QDateTime>
#include <QObject>
#include "CTranslator"
#include <QSettings>
#include <QColor>

#include "ssu.h"

// Настройки программы

namespace ssu {
    class CSettings : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(Qt::TimeSpec timeSpec READ timeSpec WRITE setTimeSpec NOTIFY timeSpecChanged)

        Q_PROPERTY(QString dateTimeFormat READ dateTimeFormat NOTIFY dateTimeFormatChanged)
        Q_PROPERTY(QString timeFormat READ timeFormat WRITE setTimeFormat NOTIFY dateTimeFormatChanged)
        Q_PROPERTY(QString dateFormat READ dateFormat WRITE setDateFormat NOTIFY dateTimeFormatChanged)

        Q_PROPERTY(int pollingInterval READ pollingInterval WRITE setPollingInterval NOTIFY pollingIntervalChanged)
        Q_PROPERTY(int measDataPollInterval READ measDataPollInterval WRITE setMeasDataPollInterval NOTIFY measDataPollIntervalChanged)
        Q_PROPERTY(int userLogPollInterval READ userLogPollInterval WRITE setUserLogPollInterval NOTIFY userLogPollIntervalChanged)
        Q_PROPERTY(int measLogPollInterval READ measLogPollInterval WRITE setMeasLogPollInterval NOTIFY measLogPollIntervalChanged)
        Q_PROPERTY(int maximumTimeDifference READ maximumTimeDifference WRITE setMaximumTimeDifference NOTIFY maximumTimeDifferenceChanged)

        Q_PROPERTY(bool updatingCardParametersOnEventEnabled READ updatingCardParametersOnEventEnabled WRITE setUpdatingCardParametersOnEventEnabled NOTIFY updatingOnEventChanged)
        Q_PROPERTY(bool updatingLogsOnEventEnabled READ updatingLogsOnEventEnabled WRITE setUpdatingLogsOnEventEnabled NOTIFY updatingOnEventChanged)

        Q_PROPERTY(bool displayingValuesWithoutCoefficients READ displayingValuesWithoutCoefficients CONSTANT)

        Q_PROPERTY(bool hasAlarmColorIndication READ hasAlarmColorIndication NOTIFY alarmColorIndicationsChanged)

    public:
        static CSettings* getInstance();

        QString dateTimeFormat() const;

        Qt::TimeSpec timeSpec() const;
        void setTimeSpec(const Qt::TimeSpec &timeSpec);

        QString timeFormat() const;
        void setTimeFormat(const QString &timeFormat);

        QString dateFormat() const;
        void setDateFormat(const QString &dateFormat);

        int pollingInterval() const;
        void setPollingInterval(int pollingInterval);

        int measDataPollInterval() const;
        void setMeasDataPollInterval(int measDataPollInterval);

        int userLogPollInterval() const;
        void setUserLogPollInterval(int userLogPollInterval);

        int maximumTimeDifference() const;
        void setMaximumTimeDifference(int maximumTimeDifference);

        static QVariant get(const QString &group, const QString &key, const QVariant &defaultValue = QVariant());
        static void set(const QString &group, const QString &key, const QVariant &value);


        bool updatingCardParametersOnEventEnabled() const;
        void setUpdatingCardParametersOnEventEnabled(bool value);

        bool updatingLogsOnEventEnabled() const;
        void setUpdatingLogsOnEventEnabled(bool value);

        int measLogPollInterval() const;
        void setMeasLogPollInterval(int measLogPollInterval);

        Q_INVOKABLE void setUpdatingOnEventEnabled(bool updatingCardParametersOnEventEnabled, bool updatingLogsOnEventEnabled);

        Q_INVOKABLE QDateTime dateTimeFromString(const QString &time) const;
        Q_INVOKABLE QDateTime getCurrentDateTime(int addSec = 0) const;

        bool displayingValuesWithoutCoefficients() const;

        Q_INVOKABLE QColor alarmColorIndication(SSU::EventStatus status) const;
        Q_INVOKABLE void setAlarmColorIndication(SSU::EventStatus status, const QColor &color);

        Q_INVOKABLE bool hasAlarmColorIndication() { return true; }

    public slots:
        void setCurrentLanguage(QLocale::Language language);

    signals:
        void timeSpecChanged(Qt::TimeSpec timeSpec);
        void dateTimeFormatChanged(const QString &format);

        void pollingIntervalChanged(int);
        void measDataPollIntervalChanged(int);
        void userLogPollIntervalChanged(int);
        void measLogPollIntervalChanged(int);
        void maximumTimeDifferenceChanged(int);

        void updatingOnEventChanged();

        void alarmColorIndicationsChanged();

    private:
        explicit CSettings(QObject *parent = nullptr);

        // Цветовая индикация по авариям различной категории срочности
        QHash <int, QColor> alarmColorIndications_;     // SSU::EventStatus


        Qt::TimeSpec timeSpec_;

        QString timeFormat_;
        QString dateFormat_;

        int pollingInterval_;                   // мс, интервал опроса
        int measDataPollInterval_;              // мс, интервал опроса данных измерений
        int measLogPollInterval_;               // мс, интервал опроса данных журнала измерений
        int userLogPollInterval_;               // мс, интервал опроса журнала пользователя

        int maximumTimeDifference_;             // мс, максимальное различие во времени между УСС и PC

        bool updatingCardParametersOnEventEnabled_;     // Включить обновление параметров платы по событию
        bool updatingLogsOnEventEnabled_;               // Включить обновление журналов по событию

        bool displayingValuesWithoutCoefficients_;   // вывод значений без коэффициентов. Потом удалить. Нужно для отладки

        void clear(const QString &group);
    };
}

#endif // CSETTINGS_H
