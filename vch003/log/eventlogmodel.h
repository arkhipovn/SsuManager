#ifndef EVENTLOGMODEL_H
#define EVENTLOGMODEL_H

#include "abstracteventlogmodel.h"

namespace ssu {
    class EventLogModel : public AbstractEventLogModel
    {
        Q_OBJECT

    public:
        explicit EventLogModel(QObject *parent = nullptr);
        Q_INVOKABLE void setInterval(const QDateTime &start, const QDateTime &end);

    public slots:
        void update() override;

    private:
        qint64 startTime_;
        qint64 endTime_;
    };
}

#endif // EVENTLOGMODEL_H


