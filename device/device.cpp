#include "device.h"

namespace ssu {

    Device::Device(SSU::DeviceType type,
                   ConnectionSettings *connectionSettings,
                   DeviceOptions *options,
                   const QMap<int, SlotConfiguration> &panelConfiguration,
                   QObject *parent) : QObject(parent),
        type_(type),
        options_(options),
        currentDateTime_(QDateTime::currentDateTimeUtc()),
        eventDescriptionList_(nullptr)
    {
        options_->setParent(this);

        channelDescription_ = new ChannelDescription(this);

        database_ = new Database;
        databaseExecutor_ = new DatabaseExecutor(database_);

        userList_ = new UserList(this);
        panel_ = new Panel(panelConfiguration, this);

        connectionSettings_ = connectionSettings;
        if(connectionSettings_->parent() == nullptr)
            connectionSettings_->setParent(this);

        // Пороги
        thresholdList_ = new ThresholdList(this);

        eventDescriptionList_ = new EventDescriptionListModel(this);

        QObject::connect(connectionSettings, &ConnectionSettings::deviceConnected, this, &Device::deviceConnected);
        QObject::connect(connectionSettings, &ConnectionSettings::deviceDisconnected, this, &Device::deviceDisconnected);
    }

    Device::~Device()
    {
        qInfo() << "~Device() start";

        if(options_ != nullptr) delete options_;
        delete databaseExecutor_;
        delete database_;

        qInfo() << "~Device() end";
    }

    Panel *Device::panel() const
    {
        return panel_;
    }

    DeviceOptions *Device::options() const
    {
        return options_;
    }

    SSU::DeviceType Device::type() const
    {
        return type_;
    }

    QDateTime Device::currentDateTime() const
    {
        return currentDateTime_;
    }

    void Device::setCurrentDateTime(const QDateTime &currentDateTime)
    {
        if(!currentDateTime.isValid()) return;
        if(currentDateTime_ != currentDateTime) {
            currentDateTime_ = currentDateTime;
            emit currentDateTimeChanged();
        }
    }

    UserList *Device::userList() const
    {
        return userList_;
    }

    ConnectionSettings *Device::connectionSettings() const
    {
        return connectionSettings_;
    }

    QString Device::name() const
    {
        return tr("SSU");
    }

    void Device::deviceConnected()
    {
        setCurrentDateTime(QDateTime::currentDateTimeUtc());
    }

    void Device::deviceDisconnected()
    {
        panel_->clear();
        userList_->clear();

        if(connectionSettings_->storeUserInformation())
            connectionSettings_->setDefaultUserLevel();
        else
            connectionSettings_->clearAuthorizationSettings();

        database_->close();
    }

    ChannelDescription *Device::channelDescription() const
    {
        return channelDescription_;
    }

    Database *Device::database() const
    {
        return database_;
    }
    
    DatabaseExecutor *Device::databaseExecutor() const
    {
        return databaseExecutor_;
    }

    EventDescriptionListModel *Device::eventDescriptionList() const
    {
        return eventDescriptionList_;
    }

    bool Device::updateThresholds() const
    {
        return true;
    }

    bool Device::changeThreshold(int channel, const DataContainer &data)
    {
        Q_UNUSED(channel)
        Q_UNUSED(data)
        return true;
    }

    bool Device::copyThresholdsToAllChannels(int channel)
    {
        Q_UNUSED(channel)
        return true;
    }

    bool Device::applyThresholdsToAllChannels(const DataContainer &data)
    {
        Q_UNUSED(data)
        return true;
    }

    bool Device::setTag(int channel, const QString &tag)
    {
        channelDescription_->setInputChannelTag(channel, tag);
        //        thresholdList_->setName(channel, channelDescription()->ssuInputChannelToString(channel));
        return true;
    }

    ThresholdList *Device::thresholdList() const
    {
        return thresholdList_;
    }
}



