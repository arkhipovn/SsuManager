#ifndef PANEL_H
#define PANEL_H

#include "cardlist.h"
#include "device/configuration/ssupanelconfiguration.h"

namespace ssu {
    class Panel : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
        Q_PROPERTY(CardList* inventory READ inventory CONSTANT)
        Q_PROPERTY(SsuPanelConfiguration* configuration READ panelConfiguration CONSTANT)

        Q_PROPERTY(QList <int> cardTypes READ cardTypes CONSTANT)

    public:
        explicit Panel(const QMap<int, SlotConfiguration> &panelConfiguration, QObject *parent = nullptr);
        ~Panel();

        CardFactory *cardFactory() const;
        Inventory *inventory() const;
        QString serialNumber() const;
        void setSerialNumber(const QString &serialNumber);
        SsuPanelConfiguration *panelConfiguration() const;

        Q_INVOKABLE void saveInventoryInformation();   // сохранение информации в csv
        Q_INVOKABLE void printInventoryInformation();  // печать информации

        void clear();

        // Вернет все доступные платы в данной группе
        Q_INVOKABLE QVariantList/*QList <Card*>*/ getCardGroup(int group, int type) const;


        //@N нужна ли сортировка? Сейчас она есть
        QList <int> cardTypes() const;                  // Возможные платы в устройстве

    signals:
        void serialNumberChanged();

    private:
        QString serialNumber_;
        CardFactory *cardFactory_;                      // Для создания объектов плат

        Inventory *inventory_;                          // список плат в панели
        SsuPanelConfiguration *panelConfiguration_;     // какие платы могут быть установлены
    };
}

#endif // PANEL_H
