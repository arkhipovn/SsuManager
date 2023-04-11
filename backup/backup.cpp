#include "backup.h"
#include "ssu.h"
#include "global.h"

#include <QDir>
#include <QDebug>

ssu::Backup::Backup(const QString &path) : path_(path)
{
    loadFilesFromDir();
}

QString ssu::Backup::path() const
{
    return path_;
}

void ssu::Backup::setPath(const QString &path)
{
    if(path_ != path) {
        path_ = path;
        loadFilesFromDir();
    }
}

void ssu::Backup::appendFile(Backup::BackupFileType type, const QString &fileName)
{
    files_.insert(type, path_ + "/" + fileName);
}

void ssu::Backup::loadFilesFromDir()
{
    files_.clear();
    if(path_.isEmpty())
        return;

    appendFile(BackupFileType::Config, DEVICE_CARD_CONFIG_FILE_NAME);
    appendFile(BackupFileType::Database, DEVICE_DATABASE_FILE_NAME);
}

QString ssu::Backup::absoluteFilePath(Backup::BackupFileType type) const
{
    auto it = files_.constFind(type);
    return it == files_.constEnd() ? ""
                                   : it.value().absoluteFilePath();
}

bool ssu::Backup::exists(BackupFileType type) const
{
    auto it = files_.constFind(type);
    return it == files_.constEnd() ? false
                                   : it.value().exists();
}

bool ssu::Backup::exists() const
{
    if(files_.isEmpty())
        return false;

    auto it = files_.constBegin();
    while(it != files_.constEnd()) {
        if(!it.value().exists())
            return false;
        ++it;
    }
    return true;
}

void ssu::Backup::clear()
{
    path_.clear();
    files_.clear();
}

void ssu::Backup::createDir()
{
    QDir dir(path_);
    if(!dir.exists()) dir.mkpath(dir.absolutePath());
}

void ssu::Backup::removePath()
{
    if(path_.isEmpty()) return;
    QDir dir(path_);
    dir.removeRecursively();
}
