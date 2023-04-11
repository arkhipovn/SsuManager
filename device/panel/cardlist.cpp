#include "cardlist.h"

ssu::CardList::CardList(CardFactory *factory, QObject *parent) : QObject(parent), factory(factory)
{

}

ssu::CardList::~CardList()
{
    clear();
    //    qDeleteAll(list);
}

QQmlListProperty<ssu::Card> ssu::CardList::data()
{
    return QQmlListProperty <Card>(this, static_cast<void*>(&list), &CardList::countData, &CardList::atData);
}

ssu::Card *ssu::CardList::append(const CardInfo &cardInfo)
{
    if(factory == nullptr) return nullptr;
    auto it = lower_bound(cardInfo.address);
    if(it != list.end() && !(cardInfo.address < (*it)->address())) return *it;   // уже есть..

    auto card = factory->create(cardInfo, this);
    if(card == nullptr) return nullptr;

    // теперь нужно добавить
    list.insert(it, card);
    emit dataChanged();
    return card;
}

void ssu::CardList::append(Card *card)
{
    if(card == nullptr) return;
    auto it = lower_bound(card->address());
    if(it != list.end() && !(card->address() < (*it)->address())) return;   // уже есть..
    card->setParent(this);

    // теперь нужно добавить
    list.insert(it, card);
    emit dataChanged();
}

void ssu::CardList::remove(Card *card)
{
    if(card == nullptr) return;
    if(list.removeOne(card))
        emit dataChanged();
    card->deleteLater();
}

void ssu::CardList::remove(int address)
{
    auto c = card(address);
    if(c == nullptr)
        return;
    remove(c);
}

void ssu::CardList::clear()
{
    if(list.isEmpty())
        return;
    for(auto card : list) card->deleteLater();
    list.clear();
    emit dataChanged();
}

bool ssu::CardList::update(const QList<CardInfo> &infoList, QList<Card *> *newCards)
{
    if(factory == nullptr) return false;
    bool changed = false;

    if(newCards != nullptr)
        newCards->clear();

    if(infoList.isEmpty()) {
        if(list.isEmpty()) return false;    // если уже пустой, то пусть так и будет
        // тут нужно очистить
        CardList::clear();
        return true;
    }

    QList <Card*> cards;
    for(auto info : infoList) {
        // посмотрим, сейчас занят ли адрес
        auto it = find(info.address);

        if(it == end()) {
            // Свободно все. Добавим
            auto card = factory->create(info, this);
            if(card == nullptr) continue;

            cards.append(card);
            changed = true;
            if(newCards != nullptr)
                newCards->append(card);
        }
        else {
            // Похоже, что адрес уже занят
            auto card = (*it);
            // Посмотрим кто там
            if(card->type() == info.type && card->serialNumber() == info.serialNumber
                    && card->hardware() == info.hardware && card->software() == info.software) {
                // Проверим серийный номер и тд
                // Вроде таже самая.. Пройдем мимо..
                list.erase(it);
                cards.append(card);
            }
            else {
                // Теперь тут другое устройство. Нужно старое удалить и добавить новое
                list.erase(it);
                card->deleteLater();
                changed = true;

                card = factory->create(info, this);
                if(card == nullptr) continue;
                cards.append(card);
                if(newCards != nullptr)
                    newCards->append(card);
            }
        }
    }

    // пройдемся по оставшимся элементам, чтобы удалить их..
    for(auto card : list) {
        card->deleteLater();
        changed = true;
    }

    std::sort(cards.begin(), cards.end(), [](Card* card1, Card* card2){
        return card1->address() < card2->address(); });
    list = cards;

    if(changed) emit dataChanged();
    return changed;
}

ssu::Card *ssu::CardList::operator[](int i)
{
    return list[i];
}

ssu::Card *ssu::CardList::at(int i)
{
    return list.at(i);
}

QList<ssu::Card*>::iterator ssu::CardList::begin()
{
    return list.begin();
}

QList<ssu::Card*>::const_iterator ssu::CardList::constBegin() const
{
    return list.constBegin();
}

QList<ssu::Card*>::iterator ssu::CardList::end()
{
    return list.end();
}

QList<ssu::Card*>::const_iterator ssu::CardList::constEnd() const
{
    return list.constEnd();
}

QList<ssu::Card*>::iterator ssu::CardList::find(int address)
{
    auto it = lower_bound(address);
    if(it != list.end() && !(address < (*it)->address())) return it;   // уже есть..
    return list.end();
}

QList<ssu::Card*>::iterator ssu::CardList::find(const QString &slot)
{
    int address = SSU::addressFromString(slot);
    if(address == -1) return end();
    return find(address);
}

QList<ssu::Card*>::const_iterator ssu::CardList::constFind(int address) const
{
    auto it = lower_bound(address);
    if(it != list.constEnd() && !(address < (*it)->address()))
        return it;   // уже есть..
    return list.constEnd();
}

QList<ssu::Card*>::const_iterator ssu::CardList::constFind(const QString &slot) const
{
    int address = SSU::addressFromString(slot);
    if(address == -1) return constEnd();
    return constFind(address);
}

bool ssu::CardList::contains(int address) const
{
    return constFind(address) != constEnd();
}

bool ssu::CardList::contains(const QString &slot) const
{
    int address = SSU::addressFromString(slot);
    if(address == -1) return false;
    return contains(address);
}

ssu::Card *ssu::CardList::card(int address) const
{
    auto it = constFind(address);
    return it == list.constEnd() ? nullptr
                                 : (*it);
}

ssu::Card *ssu::CardList::card(const QString &slot) const
{
    auto it = constFind(slot);
    return it == list.constEnd() ? nullptr
                                 : (*it);
}

ssu::Card *ssu::CardList::monitoringCard() const
{
    // Пойдем в обратном направлении. Мы знаем, что она где-то в конце
    for(int i = list.size() - 1; i >= 0 ; --i) {
        if(list.at(i)->type() == SSU::CardType::Monitoring) {
            qDebug() << "CardList::monitoringCard. ok index" << i;
            return list.at(i);
        }
    }
    return nullptr;
}

ssu::Card *ssu::CardList::findCard(SSU::CardType type) const
{
    for(int i = 0; i < list.size(); ++i) {
        if(list.at(i)->type() == type) return list.at(i);
    }
    return nullptr;
}

int ssu::CardList::numberOfCards(SSU::CardType type) const
{
    int count = 0;
    for(auto it = list.constBegin(); it != list.constEnd(); ++it) {
        if((*it)->type() == type)
            ++count;
    }
    return count;
}

QList<ssu::Card *> ssu::CardList::toList() const
{
    return list;
}

bool ssu::CardList::isEmpty() const
{
    return list.isEmpty();
}

int ssu::CardList::size() const
{
    return list.size();
}

QList<ssu::Card*>::iterator ssu::CardList::lower_bound(int address)
{
    return std::lower_bound(list.begin(), list.end(), address, [](Card* card1, int address){
        return card1->address() < address; });
}

QList<ssu::Card*>::const_iterator ssu::CardList::lower_bound(int address) const
{
    return std::lower_bound(list.constBegin(), list.constEnd(), address, [](Card* card1, int address){
        return card1->address() < address; });
}

int ssu::CardList::countData(QQmlListProperty<Card> *list)
{
    auto l = static_cast<QList<Card*>*>(list->data);
    return l->size();
}

ssu::Card *ssu::CardList::atData(QQmlListProperty<Card> *list, int index)
{
    auto l = static_cast<QList<Card*>*>(list->data);
    //    return l[index];
    return l->at(index);
}
