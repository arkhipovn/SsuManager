#ifndef EVENTDESCRIPTIONLISTMODEL_H
#define EVENTDESCRIPTIONLISTMODEL_H

#include <QObject>
#include <QPair>
#include <QAbstractListModel>

#include "eventdescription.h"
#include "eventdescriptionfilterproxymodel.h"

namespace ssu {
    class EventDescriptionListModel : public QAbstractListModel
    {
        Q_OBJECT

        Q_PROPERTY(QStringList descriptions READ descriptions NOTIFY numberOfEventsChanged)

    public:
        explicit EventDescriptionListModel(QObject *parent = nullptr);

        void append(const EventDescription &event);
        void append(const QList <EventDescription> &events);
        void update(const QList <EventDescription> &events);

        const EventDescription &operator[](int i) const;
        const EventDescription &at(int i) const;

        bool isEmpty() const;
        int size() const;

        using const_iterator = QList<ssu::EventDescription>::const_iterator;

        const_iterator constBegin() const;
        const_iterator constEnd() const;
        const_iterator constFind(int cardType, int id) const;

        void updateStatus(int cardType, int id, SSU::EventStatus status);

        SSU::EventStatus eventStatus(int cardType, int id) const;
        QString eventDescription(int cardType, int id) const;

        QHash <int, QByteArray> roleNames() const;
        QVariant data(const QModelIndex &index, int role) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

        QStringList descriptions() const;  // вернет описания событий

        friend class EventDescriptionFilterProxyModel;

    signals:
        void numberOfEventsChanged();
        void statusChanged();

    private:
        enum Role {
            IdRole = Qt::UserRole + 1,
            CardTypeRole,
            DescriptionRole,
            StatusRole
        };

        QList <EventDescription> model_;

        using iterator = QList<ssu::EventDescription>::iterator;
        iterator begin();
        iterator end();
        iterator find(int cardType, int id);

        bool appendEvent(const EventDescription &event);
    };
}

#endif // EVENTDESCRIPTIONLISTMODEL_H

