#ifndef VCH003DEVICE_H
#define VCH003DEVICE_H

#include "device/device.h"
#include "monitorring/vch003monitoring.h"
#include "log/activeeventmodel.h"

namespace ssu {
    class Vch003Device : public ssu::Device
    {
        Q_OBJECT

        Q_PROPERTY(ActiveEventModel *activeEvents READ activeEvents CONSTANT)

        Q_PROPERTY(bool measLogLoading READ measLogLoading NOTIFY deviceDataLoadingChanged)
        Q_PROPERTY(bool eventLogLoading READ eventLogLoading NOTIFY deviceDataLoadingChanged)
        Q_PROPERTY(bool userActivityLogLoading READ userActivityLogLoading NOTIFY deviceDataLoadingChanged)
        Q_PROPERTY(bool fileLoading READ fileLoading NOTIFY deviceDataLoadingChanged)

    public:
        explicit Vch003Device(ssu::ConnectionSettings *connectionSettings, QObject *parent = nullptr);
        ~Vch003Device();

        Vch003Monitoring *monitoring() const;
        QString name() const override;

        ActiveEventModel *activeEvents() const;

        bool measLogLoading() const;
        void setMeasLogLoading(bool measLogLoading);

        bool eventLogLoading() const;
        void setEventLogLoading(bool eventLogLoading);

        bool userActivityLogLoading() const;
        void setUserActivityLogLoading(bool userActivityLogLoading);

        bool fileLoading() const;
        void setFileLoading(bool fileLoading);

        void updateDownloadTime(int table);
        qint64 lastDownloadTime(int table) const;

    public slots:
        void connectToDevice() override;
        void authorization() override;
        void disconnectFromDevice() override;

        bool send(Request *request) const;
        bool sendList(QList<Request*> list) const;

    public:
        // Вот думаю, мб команды сделать наследованными от QObject? Чтобы можно было создавать их прям в qml вместо всего вот этого..
        Q_INVOKABLE bool updateInventory() const;

        Q_INVOKABLE bool updateCardEventDescription(SSU::CardType type) const;
        Q_INVOKABLE bool updateEventDescription() const;

        Q_INVOKABLE bool downloadBackup(const QString &backupDir);
        Q_INVOKABLE bool uploadBackup(const QString &backupDir);

        Q_INVOKABLE bool uploadTypeFirmware(SSU::CardType type, const QString &pathToFile);
        Q_INVOKABLE bool uploadCardFirmware(const QString &slot, const QString &pathToFile);

        // User
        Q_INVOKABLE bool updateUserList() const;                                                                    // запросить список пользователей
        Q_INVOKABLE bool addUser(const QString &username, const QString &password, SSU::UserLevel level) const;     // добавить пользователя
        Q_INVOKABLE bool deleteUser(const QString &username) const;                                                 // удалить пользователя
        Q_INVOKABLE bool changeUserPassword(const QString &username, const QString &password) const;                // поменять пароль пользователя
        Q_INVOKABLE bool blockUser(const QString &username, const QDateTime &untilDate) const;                      // блокировать пользователя
        Q_INVOKABLE bool unblockUser(const QString &username) const;                                                // разблокировать пользователя

        // Пороговые значения
        Q_INVOKABLE bool updateThresholds() const override;
        Q_INVOKABLE bool changeThreshold(int channel, const DataContainer &data) override;
        Q_INVOKABLE bool copyThresholdsToAllChannels(int channel) override;
        Q_INVOKABLE bool applyThresholdsToAllChannels(const DataContainer &data) override;
        Q_INVOKABLE bool resetThresholds() const;


        Q_INVOKABLE bool setTag(int channel, const QString &tag) override;

        Q_INVOKABLE bool changeEventStatus(SSU::CardType type, int eventId, SSU::EventStatus status);               // Обновить статус события
        Q_INVOKABLE bool updateActiveEvents() const;                                                                // Обновить список аварий


        bool downloadUserLog();             // загрузить историю действий пользователя, которых нет в базе (макс. за последние сутки)
        bool downloadEventLog();            // загрузить историю событий, которых нет в базе (макс. за последние сутки)
        bool downloadReclist();             // загрузить список измеряемых сигналов, которых нет в базе (макс. за последние сутки)


        Q_INVOKABLE bool downloadUserLog(const QDateTime &date1, const QDateTime &date2 = QDateTime());             // загрузить историю действий пользователя из базы данных устройства
        Q_INVOKABLE bool downloadEventLog(const QDateTime &date1, const QDateTime &date2 = QDateTime());            // загрузить историю событий из базы данных устройства
        Q_INVOKABLE bool downloadReclist(const QDateTime &date1, const QDateTime &date2 = QDateTime());             // загрузить список измеряемых сигналов за данный промежуток
        Q_INVOKABLE bool updateCurrentMeas();                                                                       // запросить текущие данные измерений

        Q_INVOKABLE bool updatePanelConfiguration() const;
        Q_INVOKABLE bool setPanelConfiguration(const QList <bool> &availableCards, const QList <int> &types);


        // Сброс
        Q_INVOKABLE bool resetDevice() const;
        Q_INVOKABLE bool resetCard(const QString &slot) const;
        Q_INVOKABLE bool resetToDefaultCard(const QString &slot) const;

        Q_INVOKABLE bool resetUserActivityLog() const;
        Q_INVOKABLE bool resetEventLog() const;
        Q_INVOKABLE bool resetMeasLog() const;
        Q_INVOKABLE bool resetMeasurements(int channel = -1) const;       // сбросить измерения


        //@N Потом нужно будет подумать насчет того, какие таблицы можно очищать? Вдруг будет другая версия и там другие таблицы..
        // очистка локальной базы данных
        Q_INVOKABLE void clearMeasTable();
        Q_INVOKABLE void clearUserActivityLogTable();
        Q_INVOKABLE void clearEventsLogTable();

    signals:
        void deviceDataLoadingChanged();

    protected slots:
        void deviceConnected() override;
        void deviceDisconnected() override;

        void readyRead(const QString &data);    // получены данные

    private:
        Vch003Monitoring *monitoring_;  //@N мб вынести вообще за пределы данного класса?
        ResponseHandler *responseHandler_;
        ActiveEventModel *activeEvents_;


        // Переменные показывающие, что в данный момент идет загрузка данных
        //@N Не знаю как лучше сделать. По логике бы просто смотреть есть ли в данный момент такая команда в списке..
        bool measLogLoading_;
        bool eventLogLoading_;
        bool userActivityLogLoading_;
        bool fileLoading_;

        // Время последней загрузки данных
        //@N Не знаю как лучше сделать. Раньше делал запрос в базу в запросе. Но тк все это из другого потока, то пока сделаю так
        QHash <int, qint64> lastDownloadTimeHash_;
    };
}

#endif // VCH003DEVICE_H
