#include "ssu.h"
#include <QRegularExpression>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include "global.h"


SSU::SSU(QObject *parent) : QObject(parent)
{

}

QObject *SSU::getInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new SSU;
}

void SSU::registerTypes()
{
    qRegisterMetaType <WorkingMode> ("SSU::WorkingMode");
    qRegisterMetaType <SSU::MeasDataType> ("SSU::MeasDataType");
    qRegisterMetaType <SSU::SignalType> ("SSU::SignalType");
    qRegisterMetaType <SSU::StatusThreshold> ("SSU::StatusThreshold");
    qRegisterMetaType <SSU::SSM> ("SSU::SSM");
    qRegisterMetaType <SSU::UserLevel> ("SSU::UserLevel");
    qRegisterMetaType <SSU::CardType> ("SSU::CardType");
    qRegisterMetaType <QList<SSU::CardType>> ("QList<SSU::CardType>");
    qRegisterMetaType <SSU::DeviceType> ("SSU::DeviceType");
    qRegisterMetaType <SSU::EventStatus> ("SSU::EventStatus");
    qRegisterMetaType <SSU::PllState> ("SSU::PllState");
    qRegisterMetaType <SSU::GeneratorType> ("SSU::GeneratorType");
    qRegisterMetaType <SSU::SsuType> ("SSU::SsuType");
    qRegisterMetaType <SSU::ControlMode> ("SSU::ControlMode");
}

QString SSU::maskDataDirPath()
{
    return MASK_DATA_DIR_PATH;
}

QString SSU::deviceBackupDirPath()
{
    return DEVICE_BACKUP_DIR_PATH;
}

QString SSU::windowSettingsGroup()
{
    return WINDOW_SETTINGS_GROUP;
}

QString SSU::programSettingsFile()
{
    return CONFIG_DIR_PATH + "/" + SETTINGS_INI_FILE_NAME;
}

QString SSU::programDataPath()
{
    return DATA_DIR_PATH;
}

QString SSU::programDatabaseFile(const QString &deviceDir)
{
    return DATA_DIR_PATH + "/" + deviceDir + "/" + DATABASE_FILE_NAME;
}

QString SSU::downloadTempFile()
{
    return TEMP_DIR_PATH + "/" + DOWLONAD_TEMP_FILE_NAME;
}

QString SSU::generatorTypeToString(SSU::GeneratorType type)
{
    switch (type) {
        case SSU::GeneratorType::Quartz:
            return tr("Quartz");
        case SSU::GeneratorType::Rubidium:
            return tr("Rubidium");
        default:
            break;
    }
    return "";
}

QString SSU::hemisphereToString(SSU::Hemisphere hemisphere)
{
    switch(hemisphere) {
        case Hemisphere::North:     return tr("N");
        case Hemisphere::West:      return tr("W");
        case Hemisphere::South:     return tr("S");
        case Hemisphere::East:      return tr("E");
        default:
            break;
    }
    return "";
}

QString SSU::pllStateToString(SSU::PllState state)
{
    switch(state) {
        case PllState::Warmup:        return tr("Warmup");
        case PllState::Freerun:       return tr("Freerun");
        case PllState::Tracking:      return tr("Tracking");
        case PllState::Holdover:      return tr("Holdover");
        case PllState::Adjustment:    return tr("Adjustment");
        case PllState::Error:         return tr("Error");
        case PllState::NoCapture:     return tr("No Capture");
        default:
            break;
    }

    return QString::number(static_cast<int>(state));
}

QString SSU::shortSatelliteSystemToString(SSU::SatelliteSystem system)
{
    switch(system) {
        case SSU::SatelliteSystem::GPS:           return "G";
        case SSU::SatelliteSystem::GLONASS:       return "R";
        case SSU::SatelliteSystem::SBASpayload:   return "S";
        case SSU::SatelliteSystem::Galileo:       return "E";
        case SSU::SatelliteSystem::BeiDou:        return "C";
        case SSU::SatelliteSystem::QZSS:          return "J";
        case SSU::SatelliteSystem::IRNSS:         return "I";
        default: break;
    }
    return "";
}

QString SSU::satelliteSystemToString(SSU::SatelliteSystem system)
{
    switch(system) {
        case SSU::SatelliteSystem::GPS:          return tr("GPS");
        case SSU::SatelliteSystem::GLONASS:      return tr("GLONASS");
        case SSU::SatelliteSystem::SBASpayload:  return tr("SBASpayload");
        case SSU::SatelliteSystem::Galileo:      return tr("Galileo");
        case SSU::SatelliteSystem::BeiDou:       return tr("BeiDou");
        case SSU::SatelliteSystem::QZSS:         return tr("QZSS");
        case SSU::SatelliteSystem::IRNSS:        return tr("IRNSS");
        default: break;
    }
    return tr("UnknownSystem");
}

QString SSU::cardTypeToString(SSU::CardType type)
{
    switch(type) {
        case SSU::Input:        return tr("Input card");
        case SSU::Output:       return tr("Output card");
        case SSU::OutputH:      return tr("Heavy output card");
        case SSU::OutputL:      return tr("Light output card");
        case SSU::Distribution: return tr("Distribution card");
        case SSU::Generator:    return tr("Generator card");
        case SSU::Monitoring:   return tr("Monitoring card");
        case SSU::Gnss:         return tr("GNSS");
        case SSU::Power:        return tr("Power card");
        case SSU::Stub:         return tr("Stub");
        default: break;
    }
    return "Invalid";
}

QString SSU::measDataTypeToString(SSU::MeasDataType type)
{
    switch (type) {
        case MeasDataType::Mtie:    return tr("MTIE");
        case MeasDataType::Tdev:    return tr("TDEV");
        case MeasDataType::Rf:      return tr("dF/F");
        default: break;
    }
    return "";
}

QString SSU::userLevelToString(SSU::UserLevel level)
{
    switch (level) {
        case UserLevel::User:        return tr("User");
        case UserLevel::Admin:       return tr("Administrator");
        case UserLevel::Security:    return tr("Security");
        default: break;
    }
    return "";
}

QString SSU::ssmToString(SSU::SSM ssm)
{
    switch (ssm) {
        case SSM::None:     return "None";  //return tr("None");
        case SSM::PRC:      return "PRC";   //return tr("PRC");
        case SSM::SSU_T:    return "SSU T"; //return tr("SSU T");
        case SSM::SSU_L:    return "SSU L"; //return tr("SSU L");
        case SSM::SEC:      return "SEC";   //return tr("SEC");
        case SSM::DNU:      return "DNU";   //return tr("DNU");
        default: break;
    }
    return "";
}

QString SSU::signalTypeToString(SSU::SignalType type)
{
    switch (type) {
        case SignalType::Off:     return tr("Off");
        case SignalType::MHz1:    return "1 " + tr("MHz");
        case SignalType::MHz5:    return "5 " + tr("MHz");
        case SignalType::MHz10:   return "10 " + tr("MHz");
        case SignalType::KHz2048: return "2048 " + tr("KHz");
        case SignalType::E1:      return tr("E1");
        case SignalType::Auto:    return tr("Auto", "signaltype");
        default: break;
    }
    return "";
}

QString SSU::statusThresholdToString(SSU::StatusThreshold status)
{
    switch (status) {
        case StatusThreshold::None:   return tr("None");
        case StatusThreshold::Low:    return tr("Low");
        case StatusThreshold::High:   return tr("High");
        default: break;
    }
    return "";
}

QString SSU::eventStatusToString(SSU::EventStatus status)
{
    switch(status) {
        case EventStatus::Critical: return tr("Critical");
        case EventStatus::Major:    return tr("Major");
        case EventStatus::Minor:    return tr("Minor");
        case EventStatus::Warning:  return tr("Warning");
        case EventStatus::NotAlarm: return tr("Not alarm");
        case EventStatus::Info:     return tr("Info");
        default: break;
    }
    return "";
}

QString SSU::addressToString(int address)
{
    if(address < 0)
        return "";
    return "A" + QString::number(address).rightJustified(2, '0');
}

int SSU::addressFromString(const QString &address)
{
    if(address.isEmpty()) return -1;
    QRegularExpression re("A(\\d{2})");
    QRegularExpressionMatch match = re.match(address);
    if(!match.hasMatch()) return -1;
    bool isok;
    int value = match.capturedRef(1).toInt(&isok);
    return isok ? value : -1;
}

int SSU::addressFromString(const QStringRef &address)
{
    return SSU::addressFromString(*address.string());
}

QString SSU::controlModeToString(SSU::ControlMode value)
{
    switch (value) {
        case SSU::ControlMode::Master:  return tr("Master");
        case SSU::ControlMode::Slave:   return tr("Slave");
        default: return "";
    }
}

ssu::DataContainer SSU::createEmptyData()
{
    return ssu::DataContainer();
}

QVariantList SSU::eventStatusList()
{
    return { static_cast<int>(SSU::EventStatus::Critical),
                static_cast<int>(SSU::EventStatus::Major),
                static_cast<int>(SSU::EventStatus::Minor),
                static_cast<int>(SSU::EventStatus::Warning),
                static_cast<int>(SSU::EventStatus::NotAlarm)
    };
}

QString SSU::workingModeToString(SSU::WorkingMode mode)
{
    switch(mode) {
        case WorkingMode::AutoMode:           return tr("Auto", "workingmode");
        case WorkingMode::ManualMode:         return tr("Manual", "workingmode");
        case WorkingMode::ForcedHoldover:     return tr("Forced Holdover", "workingmode");
        default:
            return "";
    }
}
