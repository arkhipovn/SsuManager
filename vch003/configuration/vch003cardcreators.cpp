#include "vch003cardcreators.h"
#include "vch003/cards/all.h"

ssu::OutputCardCreator::OutputCardCreator() : ssu::CardCreator() {}

ssu::Card *ssu::OutputCardCreator::create(int address, QObject *parent) const
{
    return new OutputCard(address, parent);
}

ssu::InputCardCreator::InputCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::InputCardCreator::create(int address, QObject *parent) const
{
    return new InputCard(address, parent);
}

ssu::GeneratorCardCreator::GeneratorCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::GeneratorCardCreator::create(int address, QObject *parent) const
{
    return new GeneratorCard(address, parent);
}

ssu::MonitoringCardCreator::MonitoringCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::MonitoringCardCreator::create(int address, QObject *parent) const
{
    return new MonitoringCard(address, parent);
}

ssu::PowerCardCreator::PowerCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::PowerCardCreator::create(int address, QObject *parent) const
{
    return new PowerCard(address, parent);
}

ssu::DistributionCardCreator::DistributionCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::DistributionCardCreator::create(int address, QObject *parent) const
{
    return new DistributionCard(address, parent);
}

ssu::GnssCardCreator::GnssCardCreator() : ssu::CardCreator() {}
ssu::Card *ssu::GnssCardCreator::create(int address, QObject *parent) const
{
    return new GnssCard(address, parent);
}


