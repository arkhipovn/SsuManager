#ifndef BACKUPLISTMODEL_H
#define BACKUPLISTMODEL_H

#include "models/filelistmodel.h"

namespace ssu {
    class BackupListModel : public FileListModel
    {
        Q_OBJECT

    public:
        explicit BackupListModel(QObject *parent = nullptr);
        ~BackupListModel();

        Q_INVOKABLE QString selectBackupDir() const;

    public slots:
        void update() override;

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        int timerId_;             // для таймера

        void start();
        void stop();

        void loadFileListFromPath() override;
    };
}

#endif // BACKUPLISTMODEL_H
