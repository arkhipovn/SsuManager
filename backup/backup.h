#ifndef BACKUP_H
#define BACKUP_H

#include <QFileInfo>
#include <QMap>

namespace ssu {
    class Backup
    {
    public:
        Backup(const QString &path);

        enum BackupFileType {
            Database,
            Config,
            None
        };

        QString path() const;
        void setPath(const QString &path);

        void loadFilesFromDir();    //@N переделать бы этот класс.. И эту функцию. Чтобы грузил реальные файлы
        QString absoluteFilePath(BackupFileType type) const;

        bool exists(BackupFileType type) const;
        bool exists() const;    // существуют все файлы

        void appendFile(BackupFileType type, const QString &fileName);

        void clear();
        void createDir();

        void removePath();

    private:
        QString path_;
        QMap <BackupFileType, QFileInfo> files_;
    };
}

#endif // BACKUP_H
