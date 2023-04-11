#ifndef USERRIGHTS_H
#define USERRIGHTS_H

#include <QObject>
#include "ssu.h"

namespace ssu {
    //@N Уйти от этого класса. Лучше добавить класс User
    class UserRights : public QObject
    {
        Q_OBJECT

    public:
        explicit UserRights(QObject *parent = nullptr);
        virtual ~UserRights() = default;

        Q_INVOKABLE virtual bool parameterControl(SSU::UserLevel level) const;
        Q_INVOKABLE virtual bool networkControl(SSU::UserLevel level) const;
        Q_INVOKABLE virtual bool deviceControl(SSU::UserLevel level) const;         // полное управление устройством. Пользователями, бэкап, обновления
        Q_INVOKABLE virtual bool eventControl(SSU::UserLevel level) const;
    };
}

#endif // USERRIGHTS_H
