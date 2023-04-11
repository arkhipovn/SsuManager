#ifndef SSUPANELCONFIGURATION_H
#define SSUPANELCONFIGURATION_H

#include <QQmlListProperty>
#include "ssuslot.h"

namespace ssu {
    class SsuPanelConfiguration : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QQmlListProperty <ssu::SsuSlot> data READ data CONSTANT)

    public:
        explicit SsuPanelConfiguration(QObject *parent = nullptr);
        explicit SsuPanelConfiguration(const QMap <int, SlotConfiguration> &slotMap,    // адрес/настройка
                                       QObject *parent = nullptr);
        ~SsuPanelConfiguration();

        void setCurrentType(int address, int type);

        SsuSlot *at(int i);
        const SsuSlot *at(int i) const;

        int size() const { return list_.size(); }

        QQmlListProperty <SsuSlot> data();

        Q_INVOKABLE ssu::SsuSlot *slot(int address) const;                  // Возвращает конфигурацию для текущего слота
        Q_INVOKABLE QList <int> getGroup(int address) const;                // Поиск группы плат
        Q_INVOKABLE QList <int> getGroupAt(int index) const;                // Поиск групп по индексу

        using const_iterator = QList <ssu::SsuSlot*>::const_iterator;
        using iterator = QList <ssu::SsuSlot*>::iterator;

        inline iterator begin() { return list_.begin(); }
        inline const_iterator constBegin() const { return list_.constBegin(); }

        inline iterator end() { return list_.end(); }
        inline const_iterator constEnd() const { return list_.constEnd(); }

    private:
        QList <SsuSlot*> list_;

        // QQmlListProperty
        static int countData(QQmlListProperty <SsuSlot> *list);
        static SsuSlot* atData(QQmlListProperty <SsuSlot> *list, int index);
    };
}

#endif // SSUPANELCONFIGURATION_H
