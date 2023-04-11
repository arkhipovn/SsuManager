#ifndef VCH003OPTIONS_H
#define VCH003OPTIONS_H

#include "device/configuration/deviceoptions.h"

namespace ssu {
    class Vch003DeviceOptions : public ssu::DeviceOptions
    {
        Q_OBJECT

        Q_PROPERTY(int kNumberOfGnssCardInChannels MEMBER kNumberOfGnssCardInChannels CONSTANT)
        Q_PROPERTY(int kNumberOfInputCardInChannels MEMBER kNumberOfInputCardInChannels CONSTANT)
        Q_PROPERTY(int kNumberOfGeneratorInChannels MEMBER kNumberOfGeneratorInChannels CONSTANT)
        Q_PROPERTY(int kNumberOfOutputCardInChannels MEMBER kNumberOfOutputCardInChannels CONSTANT)

    public:
        explicit Vch003DeviceOptions(QObject *parent = nullptr);

        static constexpr int kNumberOfGnssCardInChannels        = 3;        // Количество входных каналов в ГНСС плате
        static constexpr int kNumberOfInputCardInChannels       = 4;        // Количество входных каналов в входной плате
        static constexpr int kNumberOfGeneratorInChannels       = 18;       // Количество входных каналов в плате генератора
        static constexpr int kNumberOfOutputCardInChannels      = 20;       // Количество входных каналов в выходной плате
        static constexpr int kNumberOfOutputCardOutChannels     = 20;       // Количество выходных каналов в выходной плате


        static constexpr int kNumberOfNetworkConfigurations     = 16;       // Количество возможных конфигураций порта в ГНСС плате


        using InputLinkedChannels = QList <int>;
        static const QList <InputLinkedChannels> inputLinkedChannels;             // Для связи входа с каналами генератора и выходной платы..
    };
}

#endif // VCH003OPTIONS_H



