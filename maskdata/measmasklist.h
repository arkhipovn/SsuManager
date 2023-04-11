#ifndef MEASMASKLIST_H
#define MEASMASKLIST_H

#include "abstractmasklistmodel.h"

namespace ssu {
    class MeasMaskList : public AbstractMaskListModel
    {
        Q_OBJECT

        Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

    public:
        explicit MeasMaskList(QObject *parent = nullptr);

        QVariant data(const QModelIndex &index, int role) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        Q_INVOKABLE void createDefaultMasks(const QList <int> &intervals);

        Q_INVOKABLE bool createMask(const QString &name, const DataContainer &data);
        Q_INVOKABLE void changeMask(int index, const QString &name, const DataContainer &data);
        Q_INVOKABLE void rename(int index, const QString &name);
        Q_INVOKABLE void setMaskData(int index, const DataContainer &data);
        Q_INVOKABLE void removeMask(int index);

        MaskData mask(int index) const override;
        MaskData findMask(const QString &name) const override;

        QString path() const;
        void setPath(const QString &path);

        Q_INVOKABLE bool contains(const QString &name) const;
        Q_INVOKABLE int indexOf(const QString &name) const;

        void clear();

        QStringList maskList() const;

    public slots:
        void update();

    signals:
        void pathChanged();

    private:
        QString path_;
        QStringList maskList_;

        bool createMask(const MaskData &mask);
        DataContainer readMaskDataFromFile(const QString &maskName) const;
        void setMaskList(const QStringList &maskList);
        bool setMaskData(const QString &name, const DataContainer &data) const;
        bool rename(const QString &oldName, const QString &newName);
    };
}

#endif // MEASMASKLIST_H
