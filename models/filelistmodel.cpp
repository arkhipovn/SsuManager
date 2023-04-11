#include "filelistmodel.h"
#include "src/QCC/qccutils.h"
#include "csettings.h"
#include <QDir>
#include <QDebug>
#include <QDesktopServices>

namespace ssu {
    FileListModel::FileListModel(QObject *parent) : QAbstractListModel(parent)
    {

    }

    QHash<int, QByteArray> FileListModel::roleNames() const
    {
        static QHash<int, QByteArray> hash = {
            { Role::NameRole, "name" },
            { Role::SizeRole, "dataSize" },
            { Role::LastUpdateRole, "lastUpdate" },
        };
        return hash;
    }

    QVariant FileListModel::data(const QModelIndex &index, int role) const
    {
        if(!index.isValid())
            return QVariant();
        switch (role) {
            case Role::NameRole:
                return list_.at(index.row()).name;
            case Role::SizeRole:
                //                qInfo() << "data" << index << "sizeRole" << formatSize(fileSize(list_.at(index.row()).info));
                return formatSize(fileSize(list_.at(index.row()).info));
            case Role::LastUpdateRole: {
                return QCCUtils::qmlDateTime(list_.at(index.row()).info.lastModified(),
                                             CSettings::getInstance()->timeSpec()).toLocalTime();
            }
            default:
                break;
        }
        return QVariant();
    }

    int FileListModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return list_.size();
    }

    void FileListModel::update()
    {
        loadFileListFromPath();
    }

    void FileListModel::updateFileInfo(const QString &name)
    {
        int index = -1;
        for(int i = 0; i < list_.size(); ++i) {
            if(list_.at(i) == name) {
                index = i;
                break;
            }
        }

        if(index != -1) {
            QFileInfo info(getDir(name));
            if(info.exists())
                list_[index].info = info;
            emit dataChanged(this->index(index), this->index(index), { Role::SizeRole, Role::LastUpdateRole });
        }
    }

    QString FileListModel::path() const
    {
        return path_;
    }

    void FileListModel::setPath(const QString &path)
    {
        QString newPath = path;
        if(!newPath.isEmpty() && !newPath.endsWith(QLatin1Char('/')))
            newPath += QLatin1Char('/');
        if(path_ != newPath) {
            clear();
            path_ = newPath;
            emit pathChanged();
            if(!path_.isEmpty()) {
                update();
            }
        }
    }

    bool FileListModel::remove(const QString &name)
    {
        QDir d(path_);
        if(d.cd(name)) {
            if(d.exists()) {
                if(d.removeRecursively()) {
                    update();
                    return true;
                }
                else {
                    qWarning() << "FileListModel::remove. Data deletion error";
                }
            }
            else {
                qWarning() << "FileListModel::remove. Data does not exist";
            }
        }

        return false;
    }

    QString FileListModel::getDir(const QString &name) const
    {
        return path_ + name;
    }

    bool FileListModel::openDir(const QString &name)
    {
        QDir d(path_);
        if(d.cd(name)) {
            return QDesktopServices::openUrl(QUrl::fromLocalFile(d.absolutePath()));
        }
        return false;
    }

    void FileListModel::set(const QList<Data> &list)
    {
        if(list_ != list) {
            clear();
            if(list.isEmpty()) return;
            beginInsertRows(QModelIndex(), 0, list.size() - 1);
            list_ = list;
            endInsertRows();
        }
    }

    void FileListModel::clear()
    {
        if(list_.isEmpty())
            return;
        beginRemoveRows(QModelIndex(), 0, list_.size() - 1);
        list_.clear();
        endRemoveRows();
    }

    qint64 FileListModel::fileSize(const QFileInfo &info) const
    {
        if(info.isFile()) {
            return info.size();
        }
        else if(info.isDir()){
            qint64 size = 0;
            QDir dir(info.absoluteFilePath());
            auto fileFilters = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden;
            for(auto file : dir.entryInfoList(fileFilters)) {
                size += fileSize(file);
            }
            return size;
        }
        return 0;
    }

    QString FileListModel::formatSize(qint64 size) const
    {
        QStringList units = { tr("Bytes"), tr("KB"), tr("MB"), tr("GB") };
        int i;
        double outputSize = size;
        for(i = 0; i < units.size() - 1; ++i) {
            if(outputSize < 1024) break;
            outputSize= outputSize / 1024;
        }
        return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
    }

    bool FileListModel::Data::operator ==(const QString &other) const
    {
        return name == other;
    }

    bool FileListModel::Data::operator ==(const FileListModel::Data &other) const
    {
        if(name != other.name)
            return false;
        return info == other.info;
    }

}
