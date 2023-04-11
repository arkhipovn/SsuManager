#ifndef DEVICEOPTIONS_H
#define DEVICEOPTIONS_H

#include <QObject>
#include "userrights.h"

namespace ssu {
    class DeviceOptions : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QList <int> kMeasIntervals MEMBER kMeasIntervals CONSTANT)
        Q_PROPERTY(QList <int> kThresholdIntervals MEMBER kThresholdIntervals CONSTANT)

        Q_PROPERTY(int kNumberOfSlots MEMBER kNumberOfSlots CONSTANT)
        Q_PROPERTY(int kMaxNumberOfAuthorizationAttempts MEMBER kMaxNumberOfAuthorizationAttempts CONSTANT)

        Q_PROPERTY(UserRights *userRights READ userRights CONSTANT)

    public:
        explicit DeviceOptions(int numberOfSlots,
                               const QList <int> &measIntervals,
                               const QList <int> &thresholdIntervals,
                               int maxNumberOfAuthorizationAttempts = -1, QObject *parent = nullptr);
        virtual ~DeviceOptions() = default;

        const QList <int> kMeasIntervals;                   // интервалы измерений
        const QList <int> kThresholdIntervals;              // интервалы пороговых значений      Нужно чисто для отображения..

        const int kNumberOfSlots;                           // количество слотов
        const int kMaxNumberOfAuthorizationAttempts;        // максимальное кол-во попыток ввода пароля

        UserRights *userRights() const;

        Q_INVOKABLE qreal slotWidth(qreal panelWidth) const;
        Q_INVOKABLE virtual bool checkPassword(const QString &password);

    protected:
        UserRights *userRights_;        // для того, что определить права пользователя. Что ему возможно делать, а что нет
    };
}

#endif // DEVICEOPTIONS_H




