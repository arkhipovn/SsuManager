#include "ssupanelconfiguration.h"

ssu::SsuPanelConfiguration::SsuPanelConfiguration(QObject *parent) : QObject(parent)
{

}

ssu::SsuPanelConfiguration::SsuPanelConfiguration(const QMap<int, SlotConfiguration> &slotMap, QObject *parent) : QObject(parent)
{
    for(auto it = slotMap.constBegin(); it != slotMap.constEnd(); ++it) {
        list_.append(new SsuSlot(it.key(), it.value(), this));
    }
}

ssu::SsuPanelConfiguration::~SsuPanelConfiguration()
{
    qDeleteAll(list_);
}

void ssu::SsuPanelConfiguration::setCurrentType(int address, int type)
{
    auto s = slot(address);
    if(s != nullptr)
        s->setType(type);
}

ssu::SsuSlot *ssu::SsuPanelConfiguration::at(int i)
{
    return list_.at(i);
}

const ssu::SsuSlot *ssu::SsuPanelConfiguration::at(int i) const
{
    return list_.at(i);
}

QQmlListProperty<ssu::SsuSlot> ssu::SsuPanelConfiguration::data()
{
    return QQmlListProperty <SsuSlot>(static_cast<QObject*>(this), static_cast<void*>(&list_),
                                      &SsuPanelConfiguration::countData, &SsuPanelConfiguration::atData);
}

ssu::SsuSlot *ssu::SsuPanelConfiguration::slot(int address) const
{
    auto it = std::lower_bound(list_.begin(), list_.end(), address, [](SsuSlot *slot1, int address) {
            return slot1->address() < address; });
    return it != list_.end() && !(address < static_cast<SsuSlot*>(*it)->address()) ? *it
                                                                                   : nullptr;
}

QList<int> ssu::SsuPanelConfiguration::getGroup(int address) const
{
    QList <int> list;
    auto thisSlot = slot(address);
    if(thisSlot != nullptr) {
        for(int i = 0; i < list_.size(); ++i) {
            if(thisSlot->configuration().groupNumber() != list_.at(i)->configuration().groupNumber())
                continue;
            for(auto type : thisSlot->configuration().types()) {
                if(list_.at(i)->configuration().contains(type)) {
                    list.append(list_.at(i)->address());
                    break;
                }
            }
        }
    }

    return list;
}

QList<int> ssu::SsuPanelConfiguration::getGroupAt(int index) const
{
    QList <int> list;

    int numberOfGroup = 0;
    int prevGroup = 0;

    for(int i = 0; i < list_.size(); ++i) {
        auto group = list_.at(i)->configuration().groupNumber();
        if(prevGroup != group) {
            prevGroup = group;
            if(numberOfGroup > index) break;
        }

        if(numberOfGroup == index) {
            list.append(list_.at(i)->address());
        }
    }

    return list;
}

int ssu::SsuPanelConfiguration::countData(QQmlListProperty<SsuSlot> *list)
{
    auto l = static_cast <QList <SsuSlot*> *> (list->data);
    return l->size();
}

ssu::SsuSlot *ssu::SsuPanelConfiguration::atData(QQmlListProperty<SsuSlot> *list, int index)
{
    auto l = static_cast <QList <SsuSlot*> *> (list->data);
    return l->at(index);
}
