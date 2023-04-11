#include "programdatamanagementmodel.h"
#include "ssu.h"
#include "src/QCC/qccutils.h"
#include "csettings.h"
#include "vch003/database/vch003tables.h"
#include <QDir>

namespace ssu {
    static const int kInvalidTimer = -1;

    ProgramDataManagementModel::ProgramDataManagementModel(QObject *parent) : FileListModel(parent), timerId_(kInvalidTimer)
    {
        start();
    }

    ProgramDataManagementModel::~ProgramDataManagementModel()
    {
        stop();
    }

    void ProgramDataManagementModel::update()
    {
        start();
    }

    void ProgramDataManagementModel::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        loadFileListFromPath();
    }

    void ProgramDataManagementModel::start()
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

    void ProgramDataManagementModel::stop()
    {
        if(timerId_ != kInvalidTimer) {
            killTimer(timerId_);
            timerId_ = kInvalidTimer;
        }
    }

    void ProgramDataManagementModel::loadFileListFromPath()
    {
        if(path_.isEmpty())
            return;

        QDir dir(path_);
        QList <Data> list;

        if(dir.exists()) {
            auto dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for(const auto &dataDir : dirs) {
                QFileInfo dataFile(dataDir.absoluteFilePath() + "/" + DATABASE_FILE_NAME);
                if(dataFile.exists()) {
                    list.append({ dataDir.absoluteFilePath().mid(dataDir.canonicalPath().size() + 1), // +1 - это символ '/'
                                  dataFile
                                });
                }
            }
        }

        set(list);
    }
}

