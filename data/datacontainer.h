#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <QHash>
#include <QList>
#include <QObject>

namespace ssu {
    class DataContainer
    {
        Q_GADGET

    public:
        DataContainer();

        using Data = QList <double>;

        Q_INVOKABLE QList<double> get(int type) const;
        inline QList <double> value(int type) const { return get(type); }
        Q_INVOKABLE double value(int type, int index) const;                    // возвращает null или число

        Q_INVOKABLE void setValue(int type, int index, double value);
        Q_INVOKABLE void set(int type, const QList <double> &values);      // установить массив для данного типа

        bool isNull() const;
        void clear();

        QList <int> types() const;

        // вернет списк отличных значений (тип данных / номер интервала)
        QList <QPair <int, int>> compare(const DataContainer &other) const;

        bool operator !=(const DataContainer& other) const;

        using const_iterator = QHash <int, DataContainer::Data>::const_iterator;
        const_iterator constBegin() const { return data_.constBegin(); }
        const_iterator constEnd() const { return data_.constEnd(); }

    private:
        QHash <int, DataContainer::Data> data_;
    };
}

Q_DECLARE_METATYPE(ssu::DataContainer)

#endif // DATACONTAINER_H

