#include "currentmeasurementsmodel.h"
#include "vch003/vch003.h"

#include <QXYSeries>
#include "src/QCC/qccutils.h"
#include "csettings.h"

namespace ssu {
    CurrentMeasurementsModel::CurrentMeasurementsModel(QObject *parent) : AbstractMeasLogModel(static_cast<int>(Vch003::DatabaseTable::CurrentMeas), parent),
        dataType_(static_cast<int>(SSU::MeasDataType::Mtie))
    {
        //@N measlog
        horizontalHeader_ = QStringList( { tr("Time"),
                                           tr("Channel"),
                                           "1 " + tr("s"),
                                           "10 " + tr("s"),
                                           "100 " + tr("s"),
                                           "1000 " + tr("s"),
                                           "10000 " + tr("s"),
                                         } );
    }

    int CurrentMeasurementsModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return device_ == nullptr ? 2
                                  : 2 + device_->options()->kMeasIntervals.size();
    }

    QVariant CurrentMeasurementsModel::data(const QModelIndex &index, int role) const
    {
        QVariant v;
        if(!index.isValid())
            return v;
        if(index.row() >= rowCount())
            return v;

        switch (role) {
            case Qt::DisplayRole: {
                switch (static_cast<Columns>(index.column())) {
                    case Columns::Time: {
                        return model_.at(index.row()).time == 0 ? QVariant("")
                                                                : QCCUtils::qmlDateTime(QDateTime::fromSecsSinceEpoch(model_.at(index.row()).time, Qt::UTC),
                                                                                        CSettings::getInstance()->timeSpec()).toLocalTime();
                    }
                    case Columns::Channel: {
                        if(model_.at(index.row()).channel == -1)
                            return "";
                        if(device_ != nullptr) {
                            return device_->channelDescription()->cardInputChannelToString(SSU::Output, model_.at(index.row()).channel);
                        }
                        return model_.at(index.row()).channel;
                    }
                    default: {
                        int i = index.column() - Columns::Values;
                        double value = model_.at(index.row()).data.value(dataType_, i);
                        if(qIsNaN(value))
                            return "";
                        return QString::number(value);      //чтобы преобразовать в нужный формат..
                    }
                }
                break;
            }
            case Qt::BackgroundRole: {
                if(mask_.data.isNull())
                    return v;
                int i = index.column() - Columns::Values;
                if(i >= 0) {
                    double value = model_.at(index.row()).data.value(dataType_, i);
                    if(qIsNaN(value))
                        return v;
                    QColor errorBrush("#FECBB9");
                    auto maskValue = mask_.data.value(static_cast<int>(dataType_), i);
                    return qIsNaN(maskValue) ? v
                                             : value < qAbs(maskValue) ? v
                                                                       : errorBrush;
                }
            }
                break;
            default:
                break;
        }

        return v;
    }

    int CurrentMeasurementsModel::dataType() const
    {
        return dataType_;
    }

    void CurrentMeasurementsModel::setDataType(int dataType)
    {
        if(dataType_ != dataType) {
            dataType_ = dataType;
            emit dataTypeChanged();
            emit dataChanged(index(0, Values), index(rowCount() - 1, columnCount() - 1), { Qt::DisplayRole, Qt::BackgroundRole });
        }
    }

    void CurrentMeasurementsModel::initializeSeriesFromModel(QObject *series, int index)
    {
        QT_CHARTS_USE_NAMESPACE
        if(device_ == nullptr)
            return;
        if(series != nullptr) {
            auto xySeries = qobject_cast<QXYSeries*>(series);
            if(xySeries) {
                QVector <QPointF> points;
                if(index < model_.size()) {
                    auto list = model_.at(index).data.get(dataType_);
                    for(int i = 0; i < list.size(); ++i) {
                        double value = list.at(i);

                        if(qIsNaN(value))
                            continue;
                        if(dataType_ == static_cast<int>(SSU::MeasDataType::Mtie) ||
                                dataType_ == static_cast<int>(SSU::MeasDataType::Tdev)) {
                            if(qFuzzyCompare(value + 1, 1.)) {
                                continue;   // Тут 0..
                            }
                        }
                        points.append(QPointF(device_->options()->kMeasIntervals.at(i), value));
                    }
                }
                xySeries->replace(points);
            }
        }
    }

    void CurrentMeasurementsModel::initializeSeriesFromMask(QObject *upperSeries, QObject *lowerSeries)
    {
        QT_CHARTS_USE_NAMESPACE
        if(device_ == nullptr)
            return;
        if(upperSeries == nullptr)
            return;

        auto upperXySeries = qobject_cast<QXYSeries*>(upperSeries);
        auto lowerXySeries = qobject_cast<QXYSeries*>(lowerSeries);

        QVector <QPointF> lowerPoints;
        QVector <QPointF> upperPoints;
        auto list = mask_.data.get(dataType_);
        for(int i = 0; i < list.size(); ++i) {
            double value = list.at(i);
            if(qIsNaN(value))
                continue;
            if(dataType_ == static_cast<int>(SSU::MeasDataType::Mtie) ||
                    dataType_ == static_cast<int>(SSU::MeasDataType::Tdev)) {
                if(qFuzzyCompare(value + 1, 1.)) {
                    continue;   // Тут 0..
                }
            }

            upperPoints.append(QPointF(device_->options()->kMeasIntervals.at(i), value));

            if(lowerSeries != nullptr) {
                if(dataType_ == static_cast<int>(SSU::MeasDataType::Rf)) {
                    lowerPoints.append(QPointF(device_->options()->kMeasIntervals.at(i), -value));
                }
                else {
                    lowerPoints.append(QPointF(device_->options()->kMeasIntervals.at(i), 0));
                }
            }
        }

        upperXySeries->replace(upperPoints);
        if(lowerSeries != nullptr) {
            lowerXySeries->replace(lowerPoints);
        }
    }

}
