#ifndef USERLIST_H
#define USERLIST_H

#include <QAbstractListModel>
#include "user.h"

// Мб стоит передалать?
// Вдруг у другого устройства будет отличатся пользователь..
// То есть User сделать указателем и наследовать

namespace ssu {
    class UserList : public QAbstractListModel
    {
        Q_OBJECT

    public:
        explicit UserList(QObject *parent = nullptr);

        void set(const QVector <User> &users);
        void clear();

        QHash <int, QByteArray> roleNames() const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;

        Q_INVOKABLE void saveInventoryInformation();   // сохранение информации в csv
        Q_INVOKABLE void printInventoryInformation();  // печать информации

    private:
        enum Role {
            NameRole = Qt::UserRole + 1,
            LevelRole,
            UntilTimeRole,
            ConnectedRole
        };

        QVector <User> users;
    };
}

#endif // USERLIST_H
