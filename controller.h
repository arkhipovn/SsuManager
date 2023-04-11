#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "device/device.h"
#include "csettings.h"
#include "global.h"
#include "maskdata/measmasklist.h"

#ifdef ENABLE_APP_UPDATER
#include "updatewidgets/updaterwidget.h"
#endif

//@N переделать потом, если будет несколько устройств
namespace ssu {
    class Controller : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(Device* device READ device CONSTANT)
        Q_PROPERTY(MeasMaskList* measMaskList READ measMaskList CONSTANT)

    public:
        explicit Controller(QObject *parent = nullptr);
        ~Controller();

        Device *device() const;
        MeasMaskList *measMaskList() const;

        Q_INVOKABLE void restartApplication();

        // device пока что не используется. Заготовка на будущее. Если будет несколько устройств..
        Q_INVOKABLE void connectToDevice(Device *device, const QString &hostname, quint16 port = 5000);
        Q_INVOKABLE void authorization(Device *device, const QString &username, const QString &password, bool save = false);
        Q_INVOKABLE void disconnectFromDevice(Device *device);
        Q_INVOKABLE void setAutoConnect(Device *device, bool value);

        Q_INVOKABLE QString getOpenFileName(const QString &filters = "") const;

    public slots:
        void closeEvent();

    private:
        // Потом сделать список, наверное нужно..
        Device *device_;

        MeasMaskList *measMaskList_;        // Набор стандартных масок из папки с файлами

        void saveConnectionSettings();
        void saveAuthorizationParameters();

        //@N Потом переделать вот это. Добавить окно обновлений на qml..
#ifdef ENABLE_APP_UPDATER
    public slots:
        void openUpdateWidget();
    private slots:
        void setProgramUpdateSettings();
    private:
        UpdaterWidget *updateWidget_;
#endif
    };
}

#endif // CONTROLLER_H


