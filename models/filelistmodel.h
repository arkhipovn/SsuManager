#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QFileInfo>

namespace ssu {

    class FileListModel : public QAbstractListModel
    {
        Q_OBJECT

        Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)

    public:
        explicit FileListModel(QObject *parent = nullptr);
        virtual ~FileListModel() = default;

        QHash<int,QByteArray> roleNames() const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        QString path() const;
        void setPath(const QString &path);

        Q_INVOKABLE bool remove(const QString &name);
        Q_INVOKABLE QString getDir(const QString &name) const;
        Q_INVOKABLE bool openDir(const QString &name);

    public slots:
        virtual void update();
        void updateFileInfo(const QString &name);

    signals:
        void pathChanged();

    protected:
        QString path_;

        struct Data {
            QString name;
            QFileInfo info;

            bool operator == (const QString &other) const;
            bool operator == (const Data &other) const;
        };

        QList <Data> list_;

        void set(const QList <Data> &list);
        void clear();

        virtual void loadFileListFromPath() = 0;

        enum Role {
            NameRole,
            SizeRole,
            LastUpdateRole
        };

        qint64 fileSize(const QFileInfo &info) const;
        QString formatSize(qint64 size) const;
    };
}

#endif // FILELISTMODEL_H
