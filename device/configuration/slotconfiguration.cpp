#include "slotconfiguration.h"

ssu::SlotConfiguration::SlotConfiguration() :
    groupNumber_(0)
{

}

ssu::SlotConfiguration::SlotConfiguration(SSU::CardType type, int groupNumber) :
    groupNumber_(groupNumber),
    types_({ type })
{

}

ssu::SlotConfiguration::SlotConfiguration(const QList<SSU::CardType> &types, int groupNumber) :
    groupNumber_(groupNumber)
{
    for(int i = 0; i < types.size(); ++i)
        types_.append(static_cast<int>(types.at(i)));
}

QList<int> ssu::SlotConfiguration::types() const
{
    return types_;
}

int ssu::SlotConfiguration::groupNumber() const
{
    return groupNumber_;
}

bool ssu::SlotConfiguration::contains(int type) const
{
    return types_.contains(type);
}

bool ssu::SlotConfiguration::isEmpty()
{
    return types_.isEmpty();
}

int ssu::SlotConfiguration::type(int i) const
{
    return i < 0 || i >= types_.size() ? SSU::CardType::InvalidType
                                       : types_.at(i);
}

