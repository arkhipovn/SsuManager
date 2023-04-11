#include "channeldescription.h"
#include "ssu.h"
#include <QDebug>

namespace ssu {
    ChannelDescription::ChannelDescription(QObject *parent) : QObject(parent)
    {

    }

    void ChannelDescription::setSsuInputChannels(const SsuChannels &channels)
    {
        ssuInputChannels_ = channels;
    }

    void ChannelDescription::setCardInputChannels(const QHash<int, CardChannels> &channels)
    {
        cardInputChannels_ = channels;
    }

    void ChannelDescription::setCardOutputChannels(const QHash<int, CardChannels> &channels)
    {
        cardOutputChannels_ = channels;
    }

    void ChannelDescription::setInputChannelTag(int i, const QString &tag)
    {
        if(i < 0 || i >= ssuInputChannels_.size())
            return;
        if(ssuInputChannels_.at(i).tag != tag) {
            ssuInputChannels_[i].tag = tag;
            emit tagChanged(i);
        }
    }

    void ChannelDescription::setInputChannelTags(const QStringList &tags)
    {
        for(int i = 0; i < ssuInputChannels_.size() && i < tags.size(); ++i) {
            ssuInputChannels_[i].tag = tags.at(i);
        }
        emit tagChanged();
    }

    QString ChannelDescription::ssuInputChannelName(int i) const
    {
        return i < 0 || i >= ssuInputChannels_.size() ? ""
                                                      : ssuInputChannels_.at(i).name;
    }

    QString ChannelDescription::ssuInputChannelTag(int i) const
    {
        return i < 0 || i >= ssuInputChannels_.size() ? ""
                                                      : ssuInputChannels_.at(i).tag;
    }

    QString ChannelDescription::ssuInputChannelToString(int i, ChannelDescription::TitleSize size) const
    {
        if(i < 0 || i >= ssuInputChannels_.size())
            return "";
        if(size == TitleSize::Long && !ssuInputChannels_.at(i).tag.isEmpty()) {
            ssuInputChannels_.at(i).name + " (" + ssuInputChannels_.at(i).tag + ")";
        }
        return ssuInputChannels_.at(i).name;
    }

    QString ChannelDescription::cardChannelToString(const QHash<int, ChannelDescription::CardChannels> &channels, int cardType, int i, ChannelDescription::TitleSize size) const
    {
        if(i == 255) {
            return size == TitleSize::NumberOnly ? tr("H")
                                                 : tr("No signal"); // tr("Holdover");
        }

        auto it = channels.constFind(cardType);
        if(it == channels.constEnd() || i < 0 || i >= it.value().size())
            return QString::number(i);

        switch (size) {
            case TitleSize::Long: {
                QString str = it.value().at(i).name;
                if(!it.value().at(i).number.isEmpty())
                    str += " " + it.value().at(i).number;
                if(it.value().at(i).slot != -1)
                    str += " " + SSU::addressToString(it.value().at(i).slot);
                return str;
            }
            case TitleSize::Short: {
                QString str = it.value().at(i).name;
                if(!it.value().at(i).number.isEmpty())
                    str += " " + it.value().at(i).number;
                return str;
            }
            case TitleSize::NumberOnly:
                return it.value().at(i).number.isEmpty() ? it.value().at(i).name.left(1)
                                                         : it.value().at(i).number;
            default:
                break;
        }

        return "";
    }


    QString ChannelDescription::cardInputChannelToString(int cardType, int i, TitleSize size) const
    {
        return cardChannelToString(cardInputChannels_, cardType, i, size);
    }

    QString ChannelDescription::cardInputChannelSlot(int cardType, int i, TitleSize size) const
    {
        auto it = cardInputChannels_.constFind(cardType);
        if(it == cardInputChannels_.constEnd() || i < 0 || i >= it.value().size())
            return "";
        switch (size) {
            case TitleSize::Long:
            case TitleSize::Short:
                return SSU::addressToString(it.value().at(i).slot);
            case TitleSize::NumberOnly:
                return QString::number(it.value().at(i).slot);
            default:
                break;
        }
        return "";
    }

    QString ChannelDescription::cardOutputChannelToString(int cardType, int i, TitleSize size) const
    {
        return cardChannelToString(cardOutputChannels_, cardType, i, size);
    }


    QString ChannelDescription::cardOutputChannelSlot(int cardType, int i, TitleSize size) const
    {
        auto it = cardInputChannels_.constFind(cardType);
        if(it == cardInputChannels_.constEnd() || i < 0 || i >= it.value().size())
            return "";
        switch (size) {
            case TitleSize::Long:
            case TitleSize::Short:
                return SSU::addressToString(it.value().at(i).slot);
            case TitleSize::NumberOnly:
                return QString::number(it.value().at(i).slot);
            default:
                break;
        }
        return "";
    }

    int ChannelDescription::numberOfSsuInputChannels() const
    {
        return ssuInputChannels_.size();
    }

    ChannelDescription::CardChannel ChannelDescription::cardInputChannelAt(int cardType, int i) const
    {
        auto it = cardInputChannels_.constFind(cardType);
        if(it == cardInputChannels_.constEnd() || i < 0 || i >= it.value().size())
            return CardChannel();
        return it.value().at(i);
    }

    ChannelDescription::CardChannel ChannelDescription::cardOutputChannelAt(int cardType, int i) const
    {
        auto it = cardOutputChannels_.constFind(cardType);
        if(it == cardOutputChannels_.constEnd() || i < 0 || i >= it.value().size())
            return CardChannel();
        return it.value().at(i);
    }

    ChannelDescription::SsuChannel ChannelDescription::ssuInputChannelAt(int i) const
    {
        return i < 0 || i >= ssuInputChannels_.size() ? SsuChannel()
                                                      : ssuInputChannels_.at(i);
    }


}


