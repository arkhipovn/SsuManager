#ifndef VCH003CARDCREATORS_H
#define VCH003CARDCREATORS_H

#include "device/panel/cardcreator.h"

namespace ssu {
    //------- Создание объекта класса выходной платы -------

    class OutputCardCreator : public ssu::CardCreator
    {
    public:
        OutputCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };


    //------- Создание объекта класса платы размножителя -------

    class DistributionCardCreator : public ssu::CardCreator
    {
    public:
        DistributionCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };


    //------- Создание объекта класса входной платы -------

    class InputCardCreator : public ssu::CardCreator
    {
    public:
        InputCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };

    //------- Создание объекта класса входной платы -------

    class GeneratorCardCreator : public ssu::CardCreator
    {
    public:
        GeneratorCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };

    //------- Создание объекта класса входной платы -------

    class MonitoringCardCreator : public ssu::CardCreator
    {
    public:
        MonitoringCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };

    //------- Создание объекта класса входной платы -------

    class PowerCardCreator : public ssu::CardCreator
    {
    public:
        PowerCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };

    //------- Создание объекта класса GNSS платы -------

    class GnssCardCreator : public ssu::CardCreator
    {
    public:
        GnssCardCreator();
        Card *create(int address, QObject *parent = nullptr) const override;
    };

}

#endif // VCH003CARDCREATORS_H

