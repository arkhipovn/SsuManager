#include "datacontainer.h"
#include <QDebug>
#include "math.h"

namespace ssu {
    DataContainer::DataContainer()
    {

    }

    QList<double> DataContainer::get(int type) const
    {
        auto it = data_.constFind(type);
        return it == data_.constEnd() ? QList <double> ()
                                      : it.value();
    }

    double DataContainer::value(int type, int index) const
    {
        if(index < 0)
            return 0;
        auto it = data_.constFind(type);
        if(it == data_.constEnd())
            return NAN;
        return index >= it.value().size() ? NAN
                                          : it.value().at(index);
    }

    void DataContainer::setValue(int type, int index, double value)
    {
        if(index < 0)
            return;
        auto it = data_.find(type);

        if(it == data_.end()) {
            it = data_.insert(type, {});
        }

        if(it.value().size() <= index) {
            // Если нет данных, то добавим
            for(int i = 0; i < index + 1; ++i)
                it.value().append(0);
        }

        it.value()[index] = value;
    }

    void DataContainer::set(int type, const QList<double> &values)
    {
        auto it = data_.find(type);
        if(it == data_.end())
            data_.insert(type, values);
        else
            it.value() = values;
    }

    bool DataContainer::isNull() const
    {
        return data_.isEmpty();
    }

    void DataContainer::clear()
    {
        data_.clear();
    }

    QList<int> DataContainer::types() const
    {
        return data_.keys();
    }

    QList<QPair<int, int> > DataContainer::compare(const DataContainer &other) const
    {
        QList <QPair <int, int>> list;

        if(isNull()) {
            // Значит все изменилось.. кроме NAN
            for(auto it = other.data_.constBegin(); it != other.data_.constEnd(); ++it) {
                const auto &values = it.value();
                for(int i = 0; i < values.size(); ++i) {
                    if(!qIsNaN(values.at(i))) {
                        // Если значение определено, значит изменилось
                        list.append(qMakePair(it.key(), i));
                    }
                }
            }
        }
        else if(other.isNull()) {
            // Значит все изменилось.. кроме NAN
            for(auto it = data_.constBegin(); it != data_.constEnd(); ++it) {
                const auto &values = it.value();
                for(int i = 0; i < values.size(); ++i) {
                    if(!qIsNaN(values.at(i))) {
                        // Если значение определено, значит изменилось
                        list.append(qMakePair(it.key(), i));
                    }
                }
            }
        }
        else {
            // Нужно пройтись сначала
            QSet <int> set = data_.keys().toSet();
            set += other.data_.keys().toSet();

            for(const auto &type : set) {
                auto thisData = get(type);
                auto otherData = other.get(type);

                for(int i = 0; i < std::max(thisData.size(), otherData.size()); ++i) {
                    double thisValue = i >= thisData.size() ? NAN
                                                            : thisData.at(i);
                    double otherValue = i >= otherData.size() ? NAN
                                                              : otherData.at(i);

                    bool thisValueIsNan = qIsNaN(thisValue);
                    bool otherValueIsNan = qIsNaN(otherValue);

                    if(thisValueIsNan && otherValueIsNan) {
                        continue;
                    }
                    else if(thisValueIsNan || otherValueIsNan) {
                        list.append(qMakePair(type, i));
                    }
                    else {
                        if(!qFuzzyCompare(thisValue, otherValue))
                            list.append(qMakePair(type, i));
                    }
                }
            }
        }

        return list;
    }

    bool DataContainer::operator !=(const DataContainer &other) const
    {
        return data_ != other.data_;
    }
}
