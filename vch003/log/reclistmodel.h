#ifndef RECLISTMODEL_H
#define RECLISTMODEL_H

#include <QAbstractListModel>
#include <QDebug>

namespace ssu {
    class ReclistModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        explicit ReclistModel(QObject *parent = nullptr);

        enum Role {
            ChannelRole = Qt::UserRole + 1,
            SelectedRole
        };

        struct Record
        {
            int channel;
            bool selected = false;
            bool operator < (const Record &other) const;
            bool operator == (int channel) const;
            bool operator == (const Record &other) const;
        };

        void set(const QList <int> &channels);
        void update(const QList<int> &channels);      // С сохранением checked
        void clear();

        QHash <int, QByteArray> roleNames() const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        int rowCount(const QModelIndex &index = QModelIndex()) const;

        bool isEmpty() const;
        int size() const;

        Q_INVOKABLE void selectAll(bool value);
        Q_INVOKABLE void select(int i, bool value);
        bool isSelected(int i) const;

        const ReclistModel::Record &at(int index) const;

    private:
        QList <Record> model_;
        void set(const QList <Record> &records);
    };
}

#endif // RECLISTMODEL_H
