#include "abstracteventlogmodel.h"

#include "src/QCC/qccutils.h"
#include "ssu.h"
#include "csettings.h"
#include "vch003/database/vch003tables.h"
#include "vch003/vch003.h"
#include "vch003/configuration/vch003options.h"

namespace ssu {
    AbstractEventLogModel::AbstractEventLogModel(int tableId, QObject *parent) : AbstractDeviceLogModel(tableId, parent)
    {
        QObject::connect(CSettings::getInstance(), &CSettings::alarmColorIndicationsChanged, this, &AbstractEventLogModel::updateAlarmColorIndications);

        horizontalHeader_ = QStringList( { tr("Occurred time"),
                                           tr("Cleared time"),
                                           tr("Slot"),
                                           tr("Card"),
                                           tr("Id"),
                                           tr("Description"),
                                           tr("Status"),
                                           tr("Channel"),
                                           tr("Value"),
                                           tr("Comment")
                                         } );
    }

    QVariant AbstractEventLogModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(orientation == Qt::Horizontal) {
            switch (role) {
                case Role::EventStatusRole:
                    return AbstractDeviceLogModel::headerData(Columns::StatusColumn, orientation);
                case Role::ChannelRole:
                    return AbstractDeviceLogModel::headerData(Columns::ChannelColumn, orientation);
                case Role::CardTypeRole:
                    return AbstractDeviceLogModel::headerData(Columns::CardTypeColumn, orientation);
                default:
                    break;
            }
        }
        return AbstractDeviceLogModel::headerData(section, orientation, role);
    }

    bool AbstractEventLogModel::setDevice(Device *device)
    {
        if(AbstractDeviceLogModel::setDevice(device)) {
            if(device_ != nullptr) {
                QObject::connect(device_->eventDescriptionList(), &EventDescriptionListModel::numberOfEventsChanged,
                                 this, &AbstractEventLogModel::updateCardEvents);
                QObject::connect(device_->eventDescriptionList(), &EventDescriptionListModel::statusChanged,
                                 this, &AbstractEventLogModel::updateStatusEvents);
            }
            return true;
        }
        return false;
    }

    void AbstractEventLogModel::update()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003EventTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            auto list = table->select(device_->database(), {});
            set(list);
        }
    }

    void AbstractEventLogModel::updateStatusEvents()
    {
        emit dataChanged(index(0, Columns::StatusColumn), index(rowCount() - 1, Columns::StatusColumn), { Qt::DisplayRole, Role::EventStatusRole });
        updateAlarmColorIndications();
    }

    void AbstractEventLogModel::updateCardEvents()
    {
        emit dataChanged(index(0, Columns::DescriptionColumn), index(rowCount() - 1, Columns::DescriptionColumn), { Qt::DisplayRole });
        updateStatusEvents();
    }

    void AbstractEventLogModel::updateAlarmColorIndications()
    {
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1), { Qt::BackgroundRole });
    }

    QHash<int, QByteArray> AbstractEventLogModel::roleNames() const
    {
        static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
            { Qt::DisplayRole, "display" },
            { Qt::BackgroundRole, "background" },
            { Role::HashRole, "hash" },
            { Role::EventStatusRole, "eventStatus" },
            { Role::ChannelRole, "channel" },
            { Role::CardTypeRole, "cardType" }
        };
        return roles;
    }

    int AbstractEventLogModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return model_.size();
    }

    int AbstractEventLogModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return static_cast<int>(Columns::CommentColumn) + 1;
    }

    QVariant AbstractEventLogModel::data(const QModelIndex &index, int role) const
    {
        QVariant v;
        if(!index.isValid())
            return v;
        if(index.row() >= rowCount())
            return v;

        switch (role) {
            case Qt::DisplayRole: {
                switch (static_cast<Columns>(index.column())) {
                    case Columns::OccurredTimeColumn: {
                        return model_.at(index.row()).occurredTime == 0 ? QVariant("")
                                                                        : QCCUtils::qmlDateTime(QDateTime::fromSecsSinceEpoch(model_.at(index.row()).occurredTime, Qt::UTC),
                                                                                                CSettings::getInstance()->timeSpec()).toLocalTime();
                    }
                    case Columns::ClearedTimeColumn: {
                        return model_.at(index.row()).clearedTime == 0 ? QVariant("")
                                                                       : QCCUtils::qmlDateTime(QDateTime::fromSecsSinceEpoch(model_.at(index.row()).clearedTime, Qt::UTC),
                                                                                               CSettings::getInstance()->timeSpec()).toLocalTime();
                    }
                    case Columns::CardTypeColumn: {
                        return SSU::cardTypeToString(model_.at(index.row()).card);
                    }
                    case Columns::EventIdColumn: {
                        return model_.at(index.row()).eventId;
                    }
                    case Columns::DescriptionColumn: {
                        if(device_ != nullptr) {
                            auto it = device_->eventDescriptionList()->constFind(static_cast<int>(model_.at(index.row()).card),
                                                                                 model_.at(index.row()).eventId);
                            return it == device_->eventDescriptionList()->constEnd() ? "Unknown event №" + QString::number(model_.at(index.row()).eventId)
                                                                                     : it->description();
                        }
                        break;
                    }
                    case Columns::StatusColumn: {
                        if(device_ != nullptr) {
                            auto it = device_->eventDescriptionList()->constFind(static_cast<int>(model_.at(index.row()).card),
                                                                                 model_.at(index.row()).eventId);
                            return it == device_->eventDescriptionList()->constEnd() ? ""
                                                                                     : SSU::eventStatusToString(it->status());
                        }
                        break;
                    }
                    case Columns::ChannelColumn: {
                        int channel = model_.at(index.row()).channel;
                        if(channel < 0)
                            return "";
                        switch (static_cast<Vch003::DeviceEvent>(model_.at(index.row()).eventId)) {
                            case Vch003::DeviceEvent::OutSSMmodeChanged:
                            case Vch003::DeviceEvent::OutSSMChanged:
                            case Vch003::DeviceEvent::OutManInpChanged:
                            case Vch003::DeviceEvent::OutTypChanged:
                                return device_->channelDescription()->cardOutputChannelToString(model_.at(index.row()).card, channel);
                            default: {
                                return device_->channelDescription()->cardInputChannelToString(model_.at(index.row()).card, channel);
                            }
                        }
                    }
                    case Columns::SlotColumn: {
                        return model_.at(index.row()).slot;
                    }
                    case Columns::ValueColumn: {
                        return model_.at(index.row()).parameters;
                    }
                    case Columns::CommentColumn: {
                        return model_.at(index.row()).comment;
                    }
                    default:
                        break;
                }
                return "";
            }
            case Qt::BackgroundRole: {
                if(device_ != nullptr) {
                    auto it = device_->eventDescriptionList()->constFind(static_cast<int>(model_.at(index.row()).card),
                                                                         model_.at(index.row()).eventId);
                    if(it != device_->eventDescriptionList()->constEnd()) {
                        return CSettings::getInstance()->alarmColorIndication(it->status());
                    }
                }
                break;
            }
            case Role::HashRole:
                return qHash(model_.at(index.row()));
            case Role::EventStatusRole:
                if(device_ != nullptr) {
                    auto it = device_->eventDescriptionList()->constFind(static_cast<int>(model_.at(index.row()).card),
                                                                         model_.at(index.row()).eventId);
                    return it == device_->eventDescriptionList()->constEnd() ? -1
                                                                             : static_cast<int>(it->status());
                }
                break;
            case Role::ChannelRole:
                return model_.at(index.row()).channel;
            case Role::CardTypeRole:
                return model_.at(index.row()).card;
            default:
                break;
        }

        return v;
    }

    QString AbstractEventLogModel::comment(int row) const
    {
        return row < 0 || row >= rowCount() ? ""
                                            : model_.at(row).comment;
    }

    void AbstractEventLogModel::setComment(uint hash, const QString &comment)
    {
        if(device_ == nullptr)
            return;

        //        table_->updateRow(database(),
        //        { qMakePair(Vch003EventTable::columnName(Vch003EventTable::Columns::Comment), comment) },
        //        { Vch003EventTable::columnName(Vch003EventTable::Columns::Hash) + " = " + QString::number(hash) },
        //                          &isok);

        for(int i = 0; i < rowCount(); ++i) {
            if(qHash(model_.at(i)) != hash)
                continue;

            bool isok;
            // Нашли совпадение
            device_->databaseExecutor()->table(tableId_)->updateRow(device_->database(),
            { qMakePair(Vch003EventTable::columnName(Vch003EventTable::Columns::Comment), comment) },
            { Vch003EventTable::columnName(Vch003EventTable::Columns::Hash) + " = " + QString::number(hash) },
                                                                    &isok);

            if(isok) {
                model_[i].comment = comment;
                emit dataChanged(index(i, Columns::CommentColumn), index(i, Columns::CommentColumn), { Qt::DisplayRole });
            }

            return;
        }
    }

    uint AbstractEventLogModel::hash(int row) const
    {
        return row < 0 || row >= rowCount() ? 0
                                            : qHash(model_.at(row));
    }

    void AbstractEventLogModel::clear()
    {
        if(!model_.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, model_.size() - 1);
            model_.clear();
            endRemoveRows();
        }
    }

    void AbstractEventLogModel::set(const QList<Vch003Event> &list)
    {
        clear();
        if(!list.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, list.size() - 1);
            model_ = list;
            endInsertRows();
        }
    }
}
