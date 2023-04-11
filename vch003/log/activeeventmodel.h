#ifndef ACTIVEEVENTMODEL_H
#define ACTIVEEVENTMODEL_H

#include "abstracteventlogmodel.h"

namespace ssu {
    class ActiveEventModel : public AbstractEventLogModel
    {
        Q_OBJECT

        Q_PROPERTY(int numberOfCriticalEvents READ numberOfCriticalEvents NOTIFY eventsChanged)
        Q_PROPERTY(int numberOfMinorEvents READ numberOfMinorEvents NOTIFY eventsChanged)
        Q_PROPERTY(int numberOfMajorEvents READ numberOfMajorEvents NOTIFY eventsChanged)
        Q_PROPERTY(int numberOfWarningEvents READ numberOfWarningEvents NOTIFY eventsChanged)

        Q_PROPERTY(bool hasData READ hasData NOTIFY eventsChanged)

    public:
        explicit ActiveEventModel(QObject *parent = nullptr);

        bool setDevice(Device *device) override;

        Q_INVOKABLE bool hideEvents(const QModelIndexList &indexes);
        Q_INVOKABLE bool contains(const QString &slot, int eventId) const;

        bool hasData() const { return true; }  // для того, чтобы привязаться к изменению

        int numberOfCriticalEvents() const;
        int numberOfMinorEvents() const;
        int numberOfMajorEvents() const;
        int numberOfWarningEvents() const;

        void clear() override;

    public slots:
        void update() override;

    protected slots:
        void updateStatusEvents() override;

    signals:
        void eventsChanged();

    protected:
        int numberOfCriticalEvents_;
        int numberOfMinorEvents_;
        int numberOfMajorEvents_;
        int numberOfWarningEvents_;

        void updateNumberOfStatusEvents();
    };
}

#endif // ACTIVEEVENTMODEL_H
