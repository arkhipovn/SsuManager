#include "useractivitylogmodel.h"

#include "csettings.h"
#include "src/QCC/qccutils.h"
#include "vch003/monitorring/vch003requests.h"
#include "vch003/database/vch003tables.h"
#include "ssu.h"

#include <QSqlError>
#include <QRegularExpression>

namespace ssu {
    UserActivityLogModel::UserActivityLogModel(QObject *parent) : AbstractDeviceLogModel(static_cast<int>(Vch003::DatabaseTable::UserActivityLog), parent),
        startTime_(0),
        endTime_(0)
    {
        horizontalHeader_ = QStringList( { tr("Occurred time"),
                                           tr("Address"),
                                           tr("Username"),
                                           tr("Action"),
                                           tr("Description")
                                         } );
    }

    int UserActivityLogModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return model_.size();
    }

    int UserActivityLogModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return static_cast<int>(Columns::DescriptionColumn) + 1;
    }

    QVariant UserActivityLogModel::data(const QModelIndex &index, int role) const
    {
        QVariant v;
        if(!index.isValid())
            return v;
        if(role != Qt::DisplayRole)
            return v;
        if(index.row() >= rowCount())
            return v;

        switch (static_cast<Columns>(index.column())) {
            case Columns::OccurredTimeColumn: {
                return model_.at(index.row()).occurredTime == 0 ? QVariant("")
                                                                : QCCUtils::qmlDateTime(QDateTime::fromSecsSinceEpoch(model_.at(index.row()).occurredTime, Qt::UTC),
                                                                                        CSettings::getInstance()->timeSpec()).toLocalTime();
            }
            case Columns::AddressColumn:
                return model_.at(index.row()).address;
            case Columns::UsernameColumn:
                return model_.at(index.row()).username;
            case Columns::ActionColumn:
                return model_.at(index.row()).action;
            case Columns::DescriptionColumn:
                return descriptionFromAction(model_.at(index.row()).action);
            default:
                break;
        }
        return "";
    }

    void UserActivityLogModel::update()
    {
        if(device_ == nullptr)
            return;
        auto table = static_cast<Vch003UserActivityLogTable*>(device_->databaseExecutor()->table(tableId_));
        if(table != nullptr) {
            QStringList filters;
            if(startTime_ != 0) {
                filters.append(Vch003UserActivityLogTable::columnName(Vch003UserActivityLogTable::Columns::OccurredTime) +
                               " >= " +
                               QString::number(startTime_));
            }
            if(endTime_ != 0) {
                filters.append(Vch003UserActivityLogTable::columnName(Vch003UserActivityLogTable::Columns::OccurredTime) +
                               " <= " +
                               QString::number(endTime_));
            }

            auto list = table->select(device_->database(),
                                      filters);
            set(list);
        }
    }

    void UserActivityLogModel::setInterval(const QDateTime &start, const QDateTime &end)
    {
        qint64 startSec = start.isValid() ? start.toSecsSinceEpoch()
                                          : 0;
        qint64 endSec = end.isValid() ? end.toSecsSinceEpoch()
                                      : 0;
        if(startTime_ != startSec || endTime_ != endSec) {
            startTime_ = startSec;
            endTime_ = endSec;
            update(); // обновим..
        }
    }

    void UserActivityLogModel::clear()
    {
        if(!model_.isEmpty()) {
            beginRemoveRows(QModelIndex(), 0, model_.size() - 1);
            model_.clear();
            endRemoveRows();
        }
    }

    void UserActivityLogModel::set(const QList<Vch003UserActivity> &list)
    {
        clear();
        if(!list.isEmpty()) {
            beginInsertRows(QModelIndex(), 0, list.size() - 1);
            model_ = list;
            endInsertRows();
        }
    }

    QString UserActivityLogModel::createDescription(const QString &slot, const QString &desc, const QString &value) const
    {
        return (slot.isEmpty() ? "" : slot + ". ") + desc + (value.isEmpty() ? ""
                                                                             : ". " + tr("New value") + " \"" + value + "\""
                                                                               );
    }

    QString UserActivityLogModel::createDescription(const QString &slot, const QString &desc, const QString &channel, const QString &value) const
    {
        return (slot.isEmpty() ? "" : slot + ". ") + desc + " " + tr("for signal in input") + " " +       //tr("for signal in channel")
                channel + (value.isEmpty() ? ""
                                           : ". " + tr("New value") + " \"" + value + "\""
                                             );
    }

    QString UserActivityLogModel::descriptionFromAction(const QString &action) const
    {
        if(action.contains(vch003request::SetPanelConfiguration::cmd(), Qt::CaseInsensitive)) {
            return tr("Setting cards configuration");
        }
        if(action.contains(vch003request::UploadDb::cmd(), Qt::CaseInsensitive)) {
            return tr("Uploading a backup file: database");
        }
        if(action.contains(vch003request::UploadConfig::cmd(), Qt::CaseInsensitive)) {
            return tr("Uploading a backup file: card parameters");
        }
        if(action.contains(vch003request::SetCardEventStatus::cmd(), Qt::CaseInsensitive)) {
            return tr("Changing the status of an event");
        }
        else if(action.contains(vch003request::SetThreshold::cmd(), Qt::CaseInsensitive)) {
            return tr("Setting thresholds");
        }
        else if(action.contains(vch003request::SetAllThreshols::cmd(), Qt::CaseInsensitive)) {
            return tr("Setting thresholds for all channels");
        }
        else if(action.contains("banThrs", Qt::CaseInsensitive)) {
            return action.contains("on", Qt::CaseInsensitive) ? tr("Enable threshold disqualification")
                                                              : tr("Disable threshold disqualification");
        }
        else if(action.contains(vch003request::AddUser::cmd(), Qt::CaseInsensitive)) {            // user add      // user add LOGIN PASS 1  0 - USER, 1 - ADMIN, 2 - SEC
            return tr("Adding a new user");
        }
        else if(action.contains(vch003request::DeleteUser::cmd(), Qt::CaseInsensitive)) {      // user del        // user del USER
            return tr("Deleting a user");
        }
        else if(action.contains(vch003request::SetUserPassword::cmd(), Qt::CaseInsensitive)) {    // user setpass USER NEWPASS
            return tr("Change user password");
        }
        else if(action.contains(vch003request::BlockUser::cmd(), Qt::CaseInsensitive)) {                      // user block USER 06.08.2021 22:00:00
            return tr("Blocking a user");
        }
        else if(action.contains(vch003request::ResetDevice::cmd(), Qt::CaseInsensitive)) {
            return tr("Resetting settings to default values");
        }
        else if(action.contains(vch003request::ResetMeasurements::cmd(), Qt::CaseInsensitive)) {
            return tr("Resetting measurements");
        }
        else if(action.contains(vch003request::ResetMeasLog::cmd(), Qt::CaseInsensitive)) {
            return tr("Clearing measurement data");
        }
        else if(action.contains(vch003request::ResetUserLog::cmd(), Qt::CaseInsensitive)) {
            return tr("Clearing user activity log");
        }
        else if(action.contains(vch003request::ResetEventLog::cmd(), Qt::CaseInsensitive)) {
            return tr("Clearing the event log");
        }
        else if(action.contains("with username")) {
            return tr("An attempt to connect to a non-existent user");
        }
        else if(action.contains("with password")) {
            return tr("Password entered incorrectly");
        }
        else if(action.contains(vch003request::SetDeviceDateTime::cmd(), Qt::CaseInsensitive)) {
            return tr("Change device date and time");
        }
        else if(action.contains("typ")) {
            QRegularExpression re("(A\\d{2})? ?set typ(\\d+) (.+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Change type"), match.captured(2),
                                     SSU::signalTypeToString(vch003request::signalTypeFromString(match.capturedRef(3))));
        }
        else if(action.contains("STS")) {
            QRegularExpression re("(A\\d{2})? ?set STS(\\d+) (.+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Change threshold by status"), match.captured(2),
                                     SSU::statusThresholdToString(vch003request::statusThresholdFromString(match.capturedRef(3))));
        }
        else if(action.contains("ssm")) {
            QRegularExpression re("(A\\d{2})? ?set ssm(\\d+) (.+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Changing the SSM"), match.captured(2),
                                     SSU::ssmToString(vch003request::ssmFromString(match.capturedRef(3))));
        }
        else if(action.contains("wtRT")) {
            QRegularExpression re("(A\\d{2})? ?set wtRT(\\d+) (.+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Setting the waiting time"), match.captured(2), match.captured(3) + " s");
        }

        else if(action.contains("gmode")) {
            QRegularExpression re("(A\\d{2})? ?set gmode (.+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Changing generator mode"), SSU::workingModeToString(vch003request::workingModeFromString(match.capturedRef(2))));
        }
        else if(action.contains("priBypass")) {
            //@N
        }
        else if(action.contains("pri")) {
            QRegularExpression re("(A\\d{2})? ?set pri(T|S) (\\d+) (\\d+)", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return createDescription(match.captured(1), tr("Change of priority"), match.captured(3), match.captured(4));
        }
        else if(action.contains("set mode")) {
            QRegularExpression re("(A\\d{2}) set mode Passive", QRegularExpression::CaseInsensitiveOption);
            auto match = re.match(action);
            return tr("Change active card") + ". " + match.capturedRef(1) + " - " + tr("passive");
        }

        /*
                    *  //@N

                   reswtRT
                   Elsb
                   ssmB
                   meaM

                   enableSSM


                   network
                   banThrs

                   ssmAuto
                   ssmRAI
                   enableBypass
                   */

        else if(action.contains("swActSlot")) {
            return tr("Switch to backup slot");
        }
        else if(action.contains("set swAct")) {
            return tr("Switch to backup channel");
        }
        else if(action.contains("name")) {
            return tr("Change device name");
        }
        else if(action.contains("addr")) {
            return tr("Change device address");
        }
        else if(action.contains("port")) {
            return tr("Change device port");
        }

        return "";
    }
}
