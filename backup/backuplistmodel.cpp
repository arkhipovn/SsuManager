#include "backuplistmodel.h"
#include <QDir>
#include <QTimer>
#include <QFileDialog>
#include <QDebug>

#include "ssu.h"
#include "backup.h"

namespace ssu {
    static const int kInvalidTimer = -1;

    BackupListModel::BackupListModel(QObject *parent) : FileListModel(parent), timerId_(kInvalidTimer)
    {
        start();
    }

    BackupListModel::~BackupListModel()
    {
        stop();
    }

    QString BackupListModel::selectBackupDir() const
    {
        // Выберите каталог с резервной копией
        auto dir = QFileDialog::getExistingDirectory(0, tr("Select the directory with the backup"), path_);
        if(dir.isEmpty()) return "";
        Backup b(dir);
        b.loadFilesFromDir();
        return b.exists() ? dir : "";
    }

    void BackupListModel::update()
    {
        start();
    }

    void BackupListModel::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        loadFileListFromPath();
    }

    void BackupListModel::start()
    {
        if(timerId_ != kInvalidTimer) {
            stop();
        }
        if(path_.isEmpty())
            return;

        // считаем сразу же
        loadFileListFromPath();

        // и запустим таймер
        timerId_ = startTimer(SSU::kFileWatcherCheckInterval);
    }

    void BackupListModel::stop()
    {
        if(timerId_ != kInvalidTimer) {
            killTimer(timerId_);
            timerId_ = kInvalidTimer;
        }
    }

    void BackupListModel::loadFileListFromPath()
    {
        if(path_.isEmpty())
            return;

        QDir dir(path_);
        QList <Data> list;

        if(dir.exists()) {
            auto dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
            for(auto bDir : dirs) {
                Backup backup(bDir.absoluteFilePath());
                if(backup.exists()) {
                    list.append({ bDir.fileName(), bDir });
                }
            }
        }

        set(list);
    }

}

