#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDateTime>

#include "configuration/deviceoptions.h"
#include "configuration/connectionsettings.h"
#include "configuration/channeldescription.h"

#include "panel/panel.h"
#include "ssu.h"
#include "user/userlist.h"
#include "maskdata/thresholdlist.h"
#include "log/eventdescriptionlistmodel.h"
#include "database/database.h"

#include "database/databaseexecutor.h"

namespace ssu {
    class Device : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(SSU::DeviceType type READ type CONSTANT)
        Q_PROPERTY(Panel* panel READ panel CONSTANT)
        Q_PROPERTY(DeviceOptions* options READ options CONSTANT)
        Q_PROPERTY(UserList* userList READ userList CONSTANT)
        Q_PROPERTY(ConnectionSettings* connectionSettings READ connectionSettings CONSTANT)     // Потом сделать изменяемым. Тк вдруг появятся разные типы возможных соединений..

        Q_PROPERTY(ThresholdList* thresholdList READ thresholdList CONSTANT)
        Q_PROPERTY(ChannelDescription* channelDescription READ channelDescription CONSTANT)

        Q_PROPERTY(EventDescriptionListModel* eventDescriptionList READ eventDescriptionList CONSTANT)

        Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)

    public:
        explicit Device(SSU::DeviceType type,
                        ConnectionSettings *connectionSettings,
                        DeviceOptions *options,
                        const QMap <int, SlotConfiguration> &panelConfiguration,
                        QObject *parent = nullptr);
        virtual ~Device();

        Panel *panel() const;
        ssu::DeviceOptions *options() const;
        SSU::DeviceType type() const;

        QDateTime currentDateTime() const;
        void setCurrentDateTime(const QDateTime &currentDateTime);

        UserList *userList() const;

        ConnectionSettings *connectionSettings() const;

        Q_INVOKABLE virtual QString name() const;

        ThresholdList *thresholdList() const;
        EventDescriptionListModel *eventDescriptionList() const;
        DatabaseExecutor *databaseExecutor() const;
        Database *database() const;
        ChannelDescription *channelDescription() const;

    public:
        // Вот думаю, мб команды сделать наследованными от QObject? Чтобы можно было создавать их прям в qml вместо всего вот этого..
        Q_INVOKABLE virtual bool updateThresholds() const;
        Q_INVOKABLE virtual bool changeThreshold(int channel, const DataContainer &data);
        Q_INVOKABLE virtual bool copyThresholdsToAllChannels(int channel);
        Q_INVOKABLE virtual bool applyThresholdsToAllChannels(const DataContainer &data);

        Q_INVOKABLE virtual bool setTag(int channel, const QString &tag);


    public slots:
        virtual void connectToDevice() = 0;
        virtual void authorization() = 0;
        virtual void disconnectFromDevice() = 0;

    protected slots:
        virtual void deviceConnected();
        virtual void deviceDisconnected();

    signals:
        void currentDateTimeChanged();
        void trap(const QString &slot, const QString &description, SSU::EventStatus status, bool atEnd);

    protected:
        Database *database_;                        // База данных
        DatabaseExecutor *databaseExecutor_;        // Знает все про базу данных устройства

        SSU::DeviceType type_;                      // Тип устройства
        ChannelDescription *channelDescription_;

        DeviceOptions *options_;                    // Настройки для текущего устройства        //@N а это точно нужно?..

        Panel *panel_;                              // Инвентаризация и тд
        UserList *userList_;
        ConnectionSettings *connectionSettings_;    // Настройки соединения

        QDateTime currentDateTime_;                 // Текущее время устройства

        ThresholdList *thresholdList_;

        EventDescriptionListModel *eventDescriptionList_;
    };
}

#endif // DEVICE_H
