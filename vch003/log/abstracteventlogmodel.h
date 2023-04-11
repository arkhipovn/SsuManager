#ifndef ABSTRACTEVENTLOGMODEL_H
#define ABSTRACTEVENTLOGMODEL_H

#include "log/abstractdevicelogmodel.h"
#include "vch003/data/vch003event.h"

namespace ssu {
    class AbstractEventLogModel : public AbstractDeviceLogModel
    {
        Q_OBJECT

    public:
        explicit AbstractEventLogModel(int tableId, QObject *parent = nullptr);
        virtual ~AbstractEventLogModel() = default;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        enum Columns {
            OccurredTimeColumn = 0,
            ClearedTimeColumn,
            SlotColumn,
            CardTypeColumn,
            EventIdColumn,
            DescriptionColumn,
            StatusColumn,
            ChannelColumn,
            ValueColumn,
            CommentColumn
        };
        Q_ENUMS(Columns)

        enum Role {
            HashRole = Qt::UserRole + 1,
            EventStatusRole,
            ChannelRole,
            CardTypeRole
        };

        bool setDevice(Device *device) override;

        QHash<int, QByteArray> roleNames() const override;
        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        virtual void clear();

        Q_INVOKABLE QString comment(int row) const;
        Q_INVOKABLE void setComment(uint hash, const QString &comment);

        Q_INVOKABLE uint hash(int row) const;


        using const_iterator = QList<ssu::Vch003Event>::const_iterator;

        const_iterator constBegin() const { return model_.constBegin(); }
        const_iterator constEnd() const { return model_.constEnd(); }

    public slots:
        void update() override;

    protected slots:
        virtual void updateStatusEvents();
        virtual void updateCardEvents();
        virtual void updateAlarmColorIndications();


    protected:
        QList <Vch003Event> model_;

        void set(const QList<Vch003Event> &list);
    };
}

#endif // ABSTRACTEVENTLOGMODEL_H

