#ifndef CARDLIST_H
#define CARDLIST_H

#include <QQmlListProperty>
#include "cardfactory.h"

namespace ssu {
    class CardList : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QQmlListProperty <Card> data READ data NOTIFY dataChanged)
        Q_PROPERTY(Card* monitoringCard READ monitoringCard NOTIFY dataChanged)
        Q_PROPERTY(bool hasData READ hasData NOTIFY dataChanged)

    public:
        explicit CardList(CardFactory *factory = nullptr, QObject *parent = nullptr);
        ~CardList();

        QQmlListProperty <Card> data();

        Card *append(const CardInfo &cardInfo);
        void append(Card *card);
        void remove(Card *card);
        void remove(int address);
        void clear();

        //    void removeFirst() { list[0]->deleteLater(); list.removeFirst(); dataChanged(); }

        // Обновляет список. Если что-то обновилось, то вернет true
        // В updatedCards добавит только новые платы
        bool update(const QList<CardInfo> &infoList, QList <Card*> *newCards = nullptr);

        Card *operator[](int i);
        Card *at(int i);

        QList<Card*>::iterator begin();
        QList<Card*>::const_iterator constBegin() const;

        QList<Card*>::iterator end();
        QList<Card*>::const_iterator constEnd() const;

        QList<Card*>::iterator find(int address);
        QList<Card*>::iterator find(const QString &slot);

        QList<Card*>::const_iterator constFind(int address) const;
        QList<Card*>::const_iterator constFind(const QString &slot) const;

        QList <Card *> toList() const;
        bool isEmpty() const;
        int size() const;

        Q_INVOKABLE bool contains(int address) const;
        bool contains(const QString &slot) const;
        Q_INVOKABLE Card *card(int address) const;              // поиск платы с данным типом..
        Card *card(const QString &slot) const;
        Card *monitoringCard() const;                           // поиск платы мониторинга
        Q_INVOKABLE Card *findCard(SSU::CardType type) const;   // Поиск первой платы с данным типом

        Q_INVOKABLE int numberOfCards(SSU::CardType type) const;  // Поиск всех плат с данным типом

        bool hasData() const { return true; }  // для того, чтобы привязаться к изменению

    signals:
        void dataChanged();

    protected:
        CardFactory *factory;
        QList <Card *> list;

        QList<Card*>::iterator lower_bound(int address);
        QList<Card*>::const_iterator lower_bound(int address) const;

        // QQmlListProperty
        static int countData(QQmlListProperty <Card> *list);
        static Card* atData(QQmlListProperty <Card> *list, int index);
    };

    using Inventory = CardList;
}

#endif // CARDLIST_H
