#ifndef MEASUREMENTSMODEL_H
#define MEASUREMENTSMODEL_H

#include "currentmeasurementsmodel.h"

namespace ssu {
    // Используется как список измерений.
    //@N Переименовать бы..
    class MeasurementsModel : public CurrentMeasurementsModel
    {
        Q_OBJECT

    public:
        explicit MeasurementsModel(QObject *parent = nullptr);

        Q_INVOKABLE void set(const QList<MeasData> &list);
        Q_INVOKABLE void append(const MeasData &data);
        virtual bool setDevice(Device *device) override;

    public slots:
        void update() override;
    };
}

#endif // MEASUREMENTSMODEL_H
