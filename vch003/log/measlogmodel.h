#ifndef MEASLOGMODEL_H
#define MEASLOGMODEL_H

#include "abstractmeaslogmodel.h"

namespace ssu {
    class MeasLogModel : public AbstractMeasLogModel
    {
        Q_OBJECT

    public:
        explicit MeasLogModel(QObject *parent = nullptr);

        Q_INVOKABLE void setInterval(const QDateTime &start, const QDateTime &end, int decimation);

        Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    public slots:
        void update() override;

    protected slots:
        virtual void updateModel();

    private:
        qint64 startTime_;
        qint64 endTime_;
        int decimation_;    // в часах
    };
}

#endif // MEASLOGMODEL_H
