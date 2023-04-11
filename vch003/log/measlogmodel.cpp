#include "measlogmodel.h"
#include "vch003/vch003.h"
#include "vch003/database/vch003tables.h"

#include "src/QCC/qccutils.h"
#include "csettings.h"

namespace ssu {

    MeasLogModel::MeasLogModel(QObject *parent) : AbstractMeasLogModel(static_cast<int>(Vch003::DatabaseTable::MeasLog), parent),
        startTime_(0),
        endTime_(0),
        decimation_(0)
    {
        //@N measlog
        horizontalHeader_ = QStringList( { tr("Time"),
                                           tr("Channel"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Mtie) + " 1 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Mtie) + " 10 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Mtie) + " 100 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Mtie) + " 1000 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Mtie) + " 10000 " + tr("s"),

                                           SSU::measDataTypeToString(SSU::MeasDataType::Tdev) + " 1 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Tdev) + " 10 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Tdev) + " 100 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Tdev) + " 1000 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Tdev) + " 10000 " + tr("s"),

                                           SSU::measDataTypeToString(SSU::MeasDataType::Rf) + " 1 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Rf) + " 10 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Rf) + " 100 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Rf) + " 1000 " + tr("s"),
                                           SSU::measDataTypeToString(SSU::MeasDataType::Rf) + " 10000 " + tr("s"),
                                         } );
    }

    void MeasLogModel::setInterval(const QDateTime &start, const QDateTime &end, int decimation)
    {
        qint64 startSec = start.isValid() ? start.toSecsSinceEpoch()
                                          : 0;
        qint64 endSec = end.isValid() ? end.toSecsSinceEpoch()
                                      : 0;
        if(startTime_ != startSec || endTime_ != endSec || decimation_ != decimation) {
            startTime_ = startSec;
            endTime_ = endSec;
            decimation_ = decimation;
            update(); // обновим..
        }
    }

    int MeasLogModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return device_ == nullptr ? 2
                                  : 2 + device_->options()->kMeasIntervals.size() * 3;  //@N measlog
    }

    QVariant MeasLogModel::data(const QModelIndex &index, int role) const
    {
        QVariant v;
        if(device_ == nullptr)
            return v;
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
                        int featureNumber = (index.column() - Columns::Values) / device_->options()->kMeasIntervals.size();
                        int i = index.column() - Columns::Values - featureNumber * device_->options()->kMeasIntervals.size();

                        SSU::MeasDataType dataType = SSU::MeasDataType::Invalid;
                        switch (featureNumber) {
                            case 0: dataType = SSU::MeasDataType::Mtie; break;    // MTIE
                            case 1: dataType = SSU::MeasDataType::Tdev; break;    // TDEV
                            case 2: dataType = SSU::MeasDataType::Rf; break;      // RF
                            default: break;
                        }

                        if(dataType == SSU::MeasDataType::Invalid) {
                            return "";
                        }
                        else {
                            double value = model_.at(index.row()).data.value(static_cast<int>(dataType), i);
                            if(qIsNaN(value))
                                return "";
                            return QString::number(value);      //чтобы преобразовать в нужный формат..
                        }
                    }
                }
                break;
            }
            case Qt::BackgroundRole: {
                if(mask_.data.isNull())
                    return v;
                if(index.column() > Columns::Values) {
                    int featureNumber = (index.column() - Columns::Values) / device_->options()->kMeasIntervals.size();
                    int i = index.column() - Columns::Values - featureNumber * device_->options()->kMeasIntervals.size();

                    SSU::MeasDataType dataType = SSU::MeasDataType::Invalid;
                    switch (featureNumber) {
                        case 0: dataType = SSU::MeasDataType::Mtie; break;    // MTIE
                        case 1: dataType = SSU::MeasDataType::Tdev; break;    // TDEV
                        case 2: dataType = SSU::MeasDataType::Rf; break;      // RF
                        default: break;
                    }

                    if(dataType == SSU::MeasDataType::Invalid) {
                        return v;
                    }
                    else {
                        double value = model_.at(index.row()).data.value(static_cast<int>(dataType), i);
                        if(qIsNaN(value))
                            return v;
                        QColor errorBrush("#FECBB9");
                        auto maskValue = mask_.data.value(static_cast<int>(dataType), i);
                        return qIsNaN(maskValue) ? v
                                                 : value < qAbs(maskValue) ? v
                                                                           : errorBrush;
                    }
                }
            }
                break;
            default:
                break;
        }

        return v;
    }

    void MeasLogModel::update()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003MeasurementLogTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            reclist_->update(table->readReclist(device_->database(), startTime_, endTime_, decimation_ * 3600));    // Тк тут в часах
        }
        updateModel();
    }

    void MeasLogModel::updateModel()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003MeasurementLogTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            QList <MeasData> list;
            for(int i = 0; i < reclist_->size(); ++i) {
                const auto &rec = reclist_->at(i);
                if(rec.selected) {
                    list.append(table->select(device_->database(), rec.channel, startTime_, endTime_, decimation_ * 3600));               // Тк тут в часах
                }
            }
            set(list);
        }
    }

}
