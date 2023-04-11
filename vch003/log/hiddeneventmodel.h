#ifndef HIDDENEVENTMODEL_H
#define HIDDENEVENTMODEL_H

#include "abstracteventlogmodel.h"

namespace ssu {
    class HiddenEventModel : public AbstractEventLogModel
    {
        Q_OBJECT

    public:
        explicit HiddenEventModel(QObject *parent = nullptr);
        Q_INVOKABLE bool showEvents(const QModelIndexList &indexes);
        Q_INVOKABLE bool showAllEvents();
    };
}

#endif // HIDDENEVENTMODEL_H
