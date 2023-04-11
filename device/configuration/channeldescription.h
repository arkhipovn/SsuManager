#ifndef CHANNELDESCRIPTION_H
#define CHANNELDESCRIPTION_H

#include <QObject>
#include <QHash>

// Класс для хранения названия каналов плат и входных каналов SSU

//@N
// Пока не придумал как сделать это лучше..
// Так-то лучше бы задавать имена в классах для каналов. Но тогда чтобы в каком-то месте программы получить
// имя нужно будет создавать класс блока. И у него запрашивать канала и его имя..

// Назвать бы подругому..

namespace ssu {

    class ChannelDescription : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(bool hasTag READ hasTag NOTIFY tagChanged)

    public:
        explicit ChannelDescription(QObject *parent = nullptr);

        enum class TitleSize {
            Long,           // полная запись.       Имя номер слот
            Short,          // короткая запись      Имя номер
            NumberOnly      //                      Номер
        };
        Q_ENUMS(TitleSize)

        struct CardChannel {
            QString name;           // Пример: Input/GNSS
            QString number;         // Пример: 1/2/3/G
            int slot;               // Пример: 0/1      Если нужна привязка к слоту..
            CardChannel() : name(""), number(""), slot(-1) {}
            CardChannel(const QString &name, const QString &number = "", int slot = -1) : name(name), number(number), slot(slot) {}
        };

        struct SsuChannel {
            QString name;       // Название канала
            QString tag;        // Описание канала
            SsuChannel() : name(""), tag("") {}
            SsuChannel(const QString &name, const QString &tag = "") : name(name), tag(tag) {}
        };

        using CardChannels = QList <CardChannel>;
        using SsuChannels = QList <SsuChannel>;


        SsuChannel ssuInputChannelAt(int i) const;
        CardChannel cardInputChannelAt(int cardType, int i) const;
        CardChannel cardOutputChannelAt(int cardType, int i) const;


        // Так-то вот это нужно сделать 1 раз..
        void setSsuInputChannels(const SsuChannels &channels);
        void setCardInputChannels(const QHash<int, CardChannels> &channels);
        void setCardOutputChannels(const QHash<int, CardChannels> &channels);

        void setInputChannelTag(int i, const QString &tag);
        void setInputChannelTags(const QStringList &tags);

        Q_INVOKABLE int numberOfSsuInputChannels() const;   // количество входных каналов у SSU

        Q_INVOKABLE QString ssuInputChannelToString(int i, TitleSize size = TitleSize::Long) const;
        Q_INVOKABLE QString ssuInputChannelName(int i) const;
        Q_INVOKABLE QString ssuInputChannelTag(int i) const;

        Q_INVOKABLE QString cardInputChannelToString(int cardType, int i, TitleSize size = TitleSize::Long) const;
        Q_INVOKABLE QString cardInputChannelSlot(int cardType, int i, TitleSize size = TitleSize::Long) const;

        Q_INVOKABLE QString cardOutputChannelToString(int cardType, int i, TitleSize size = TitleSize::Long) const;
        Q_INVOKABLE QString cardOutputChannelSlot(int cardType, int i, TitleSize size = TitleSize::Long) const;

        bool hasTag() const { return true; }

    signals:
        void tagChanged(int i = -1);         // Если i == -1, значит обновились у все каналов. Если >= 0, то у конкретного

    private:
        SsuChannels ssuInputChannels_;                   // Список входных каналов SSU

        // Для каждой платы перечислить нужно..
        QHash <int, ChannelDescription::CardChannels> cardInputChannels_;          // Входные каналы каждой платы
        QHash <int, ChannelDescription::CardChannels> cardOutputChannels_;         // Выходные каналы каждой платы

        QString cardChannelToString(const QHash <int, ChannelDescription::CardChannels> &channels, int cardType, int i, TitleSize size) const;
    };
}

#endif // CHANNELDESCRIPTION_H

