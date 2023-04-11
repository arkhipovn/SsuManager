#ifndef CURRENTMEASUREMENTSMODEL_H
#define CURRENTMEASUREMENTSMODEL_H

#include "abstractmeaslogmodel.h"

namespace ssu {
    // Текущие измерения. Берутся из БД

    class CurrentMeasurementsModel : public AbstractMeasLogModel
    {
        Q_OBJECT

        Q_PROPERTY(int dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)

    public:
        explicit CurrentMeasurementsModel(QObject *parent = nullptr);
        virtual ~CurrentMeasurementsModel() = default;

        Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        int dataType() const;
        void setDataType(int dataType);

    public slots:
        void initializeSeriesFromModel(QObject *series, int index);
        void initializeSeriesFromMask(QObject *upperSeries, QObject *lowerSeries = nullptr);

    signals:
        void dataTypeChanged();

    protected:
        int dataType_;
    };
}

#endif // CURRENTMEASUREMENTSMODEL_H
