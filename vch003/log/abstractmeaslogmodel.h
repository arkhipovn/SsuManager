#ifndef ABSTRACTMEASLOGMODEL_H
#define ABSTRACTMEASLOGMODEL_H

#include "log/abstractdevicelogmodel.h"
#include "data/measdata.h"
#include "maskdata/maskdata.h"
#include "reclistmodel.h"

namespace ssu {
    class AbstractMeasLogModel : public AbstractDeviceLogModel
    {
        Q_OBJECT

        Q_PROPERTY(MaskData mask READ mask WRITE setMask NOTIFY maskChanged)
        Q_PROPERTY(bool useMask READ useMask NOTIFY maskChanged)
        Q_PROPERTY(ReclistModel* reclist READ reclist CONSTANT)

        Q_PROPERTY(bool shortHeaderTitles READ shortHeaderTitles WRITE setShortHeaderTitles)

    public:
        explicit AbstractMeasLogModel(int tableId, QObject *parent = nullptr);
        virtual ~AbstractMeasLogModel() = default;

        enum Columns {
            Time = 0,
            Channel,
            Values
        };
        Q_ENUMS(Columns)

        QHash<int, QByteArray> roleNames() const override;
        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        virtual void clear();

        bool useMask() const;

        MaskData mask() const;
        void setMask(const MaskData &mask);
        Q_INVOKABLE void clearMask();

        ReclistModel *reclist() const;

        Q_INVOKABLE QList <MeasData> getMeasData(const QModelIndexList &indexes) const;

        bool shortHeaderTitles() const;
        void setShortHeaderTitles(bool shortHeaderTitles);

    public slots:
        void update() override;

    protected slots:
        virtual void updateModel();

    signals:
        void maskChanged();

    protected:
        QList <MeasData> model_;
        MaskData mask_;
        ReclistModel *reclist_;
        bool shortHeaderTitles_;

        virtual void set(const QList<MeasData> &list);
    };
}

#endif // ABSTRACTMEASLOGMODEL_H
