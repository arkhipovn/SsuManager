#ifndef USERACTIVITYLOGMODEL_H
#define USERACTIVITYLOGMODEL_H

#include "log/abstractdevicelogmodel.h"
#include "vch003/data/vch003useractivity.h"

namespace ssu {
    class UserActivityLogModel : public AbstractDeviceLogModel
    {
        Q_OBJECT

    public:
        explicit UserActivityLogModel(QObject *parent = nullptr);

        enum class Columns {
            OccurredTimeColumn = 0,
            AddressColumn,
            UsernameColumn,
            ActionColumn,
            DescriptionColumn
        };
        Q_ENUMS(Columns)

        Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        Q_INVOKABLE void setInterval(const QDateTime &start, const QDateTime &end);

        void clear();

    public slots:
        void update() override;
        
    protected:
        QList <Vch003UserActivity> model_;

        void set(const QList<Vch003UserActivity> &list);

    private:
        //@N расшифровка действий в журнале пользователя. Переделать по нормальному..
        QString createDescription(const QString &slot, const QString &desc, const QString &value) const;
        QString createDescription(const QString &slot, const QString &desc, const QString &channel, const QString &value) const;
        //@N от этого вообще нужно как-то уходить..
        QString descriptionFromAction(const QString &action) const;

        qint64 startTime_;
        qint64 endTime_;
    };
}

#endif // USERACTIVITYLOGMODEL_H
