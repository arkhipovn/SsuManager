#include "vch003card.h"
#include "csettings.h"

namespace ssu {
    Vch003Card::Vch003Card(int type, int address, QObject *parent) : Card(type, address, parent)
    {

    }

    void Vch003Card::parseParameters(const QString &parameterList)
    {
        Q_UNUSED(parameterList)
    }

    QList<Request *> Vch003Card::requestList() const
    {
        if(type_ == SSU::CardType::InvalidType || type_ == SSU::CardType::Stub)
            return {};
        auto cmd = new vch003request::GetCardParameters(slot());
        cmd->setDelay(CSettings::getInstance()->pollingInterval());
        return { cmd };
    }

    QList<Request *> Vch003Card::initCommandList() const
    {
        if(type_ == SSU::CardType::InvalidType || type_ == SSU::CardType::Stub)
            return {};
        return { new vch003request::GetCardParameters(slot()) };
    }

    QList<Request *> Vch003Card::updateCardParametersCommand() const
    {
        return initCommandList();
    }

}
