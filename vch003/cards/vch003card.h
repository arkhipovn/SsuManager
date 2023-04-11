#ifndef VCH003CARD_H
#define VCH003CARD_H

#include "device/panel/card.h"
#include "vch003/monitorring/vch003requests.h"

namespace ssu {
    class Vch003Card : public Card
    {
        Q_OBJECT

    public:
        explicit Vch003Card(int type = 0, int address = -1, QObject *parent = nullptr);

        //@N не знаю.. Нужно, наверное, убрать это отсюда. Вынести в отдельный класс или еще что-то
        virtual void parseParameters(const QString &parameterList);

        virtual QList <Request*> requestList() const;           // список для периодического опроса
        virtual QList <Request*> initCommandList() const;       // команды для инициализации платы

        Q_INVOKABLE QList<Request *> updateCardParametersCommand() const;

    protected:
    };
}

#endif // VCH003CARD_H
