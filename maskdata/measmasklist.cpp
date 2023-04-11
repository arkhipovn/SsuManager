#include "measmasklist.h"

#include <QDir>
#include <QSettings>
#include <QFile>
#include <QDebug>

#include "global.h"
#include "ssu.h"

namespace ssu {
    MeasMaskList::MeasMaskList(QObject *parent) : AbstractMaskListModel(parent)
    {
        update();
    }

    QVariant MeasMaskList::data(const QModelIndex &index, int role) const
    {
        if(!index.isValid())
            return QVariant();
        switch (role) {
            case Role::NameRole:
                return maskList_.at(index.row());
            case Role::MaskDataRole:
                return QVariant::fromValue(readMaskDataFromFile(maskList_.at(index.row())));
            case Role::MaskRole: {
                return QVariant::fromValue(mask(index.row()));
            }
            default:
                break;
        }
        return QVariant();
    }

    int MeasMaskList::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return maskList_.size();
    }

    void MeasMaskList::createDefaultMasks(const QList<int> &intervals)
    {
        if(path_.isEmpty())
            return;

        // Маски по умолчанию
        G811 g811;
        createMask(MaskData(&g811, intervals));

        G811d1 g811d1;
        createMask(MaskData(&g811d1, intervals));

        G812Tolerance g812Tolerance;
        createMask(MaskData(&g812Tolerance, intervals));

        G812LockedMode g811LockedMode;
        createMask(MaskData(&g811LockedMode, intervals));
    }

    bool MeasMaskList::createMask(const QString &name, const DataContainer &data)
    {
        if(maskList_.contains(name)) {
            return false;
        }
        if(path_.isEmpty())
            return false;
        QDir dir(path_);
        if(!dir.exists()) dir.mkpath(dir.absolutePath());

        if(setMaskData(name, data)) {
            update();   // Тут мб не обновлять все?
            return true;
        }
        return false;
    }

    void MeasMaskList::changeMask(int index, const QString &name, const DataContainer &data)
    {
        if(index < 0 || index >= maskList_.size())
            return;
        setMaskData(maskList_.at(index), data);
        rename(maskList_.at(index), name);
    }

    void MeasMaskList::setMaskData(int index, const DataContainer &data)
    {
        if(index < 0 || index >= maskList_.size())
            return;
        if(setMaskData(maskList_.at(index), data)) {
            emit dataChanged(this->index(index), this->index(index), { Role::MaskRole, Role::MaskDataRole });
        }
    }

    void MeasMaskList::rename(int index, const QString &name)
    {
        if(index < 0 || index >= maskList_.size())
            return;
        if(rename(maskList_.at(index), name)) {
            maskList_[index] = name;
            emit dataChanged(this->index(index), this->index(index), { Role::NameRole, Role::MaskRole });
        }
    }

    void MeasMaskList::removeMask(int index)
    {
        if(index < 0 || index >= maskList_.size())
            return;

        const QString absName = path_ + maskList_.at(index) + "." + MASK_FILE_SUFFIX;
        if(QFile::remove(absName)) {
            beginRemoveRows(QModelIndex(), index, index);
            maskList_.removeAt(index);
            endRemoveRows();
        }
    }

    MaskData MeasMaskList::mask(int index) const
    {
        if(index < 0 || index >= maskList_.size())
            return MaskData("Invalid");
        return MaskData(maskList_.at(index),
                        readMaskDataFromFile(maskList_.at(index)));
    }

    MaskData MeasMaskList::findMask(const QString &name) const
    {
        for(int i = 0; i < maskList_.size(); ++i) {
            if(maskList_.at(i) == name) {
                return mask(i);
            }
        }
        return MaskData("Invalid");
    }

    QString MeasMaskList::path() const
    {
        return path_;
    }

    void MeasMaskList::setPath(const QString &path)
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

    bool MeasMaskList::contains(const QString &name) const
    {
        return maskList_.contains(name);
    }

    int MeasMaskList::indexOf(const QString &name) const
    {
        return maskList_.indexOf(name);
    }

    void MeasMaskList::clear()
    {
        if(maskList_.isEmpty())
            return;
        beginRemoveRows(QModelIndex(), 0, maskList_.size() - 1);
        maskList_.clear();
        endRemoveRows();
    }

    bool MeasMaskList::setMaskData(const QString &name, const DataContainer &data) const
    {
        const QString absName = path_ + name + "." + MASK_FILE_SUFFIX;
        QSettings maskFile(absName, QSettings::IniFormat);
        maskFile.beginGroup(MASK_INFO_GROUP);
        for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
            QString str;
            for(int i = 0; i < it.value().size(); ++i) {
                str.append(QString::number(it.value().at(i)));
                if(i < it.value().size() - 1) str.append("/");
            }
            maskFile.setValue(QString::number(it.key()), str);
        }
        maskFile.endGroup();
        return true;
    }

    bool MeasMaskList::rename(const QString &oldName, const QString &newName)
    {
        if(oldName == newName)
            return false;
        return QFile::rename(path_ + oldName + "." + MASK_FILE_SUFFIX,
                             path_ + newName + "." + MASK_FILE_SUFFIX);
    }

    void MeasMaskList::setMaskList(const QStringList &maskList)
    {
        if(maskList_ != maskList) {
            clear();
            if(maskList.isEmpty())
                return;
            beginInsertRows(QModelIndex(), 0, maskList.size() - 1);
            maskList_ = maskList;
            endInsertRows();
        }
    }

    void MeasMaskList::update()
    {
        if(path_.isEmpty())
            return;

        QDir dir(path_);
        QStringList names;
        if(dir.exists()) {
            auto files = dir.entryInfoList({ QString("*.") + MASK_FILE_SUFFIX }, QDir::Files, QDir::SortFlag::Name);
            for(auto &f : files) {
                names.append(f.completeBaseName());
            }
        }

        setMaskList(names);
    }

    QStringList MeasMaskList::maskList() const
    {
        return maskList_;
    }

    bool MeasMaskList::createMask(const MaskData &mask)
    {
        return createMask(mask.name, mask.data);
    }

    DataContainer MeasMaskList::readMaskDataFromFile(const QString &maskName) const
    {
        DataContainer container;
        const QString absName = path_ + maskName + "." + MASK_FILE_SUFFIX;

        if(QFile::exists(absName)) {
            QSettings maskFile(absName, QSettings::IniFormat);
            maskFile.beginGroup(MASK_INFO_GROUP);

            bool isok;
            for(const QString &key : maskFile.allKeys()) {

                int dataType = key.toInt(&isok);
                if(!isok)
                    continue;
                auto value = maskFile.value(key).toString();
                auto list = value.split("/");
                bool isok;
                QList <double> d;
                for(auto &l : list) {
                    double value = l == "-" ? 0 : l.toDouble(&isok);
                    d.append(isok ? value : 0);
                }

                container.set(dataType, d);
            }

            maskFile.endGroup();
        }

        return container;
    }
}
