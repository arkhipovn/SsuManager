#include "vch003.h"

QObject *Vch003::getInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Vch003;
}

void Vch003::registerTypes()
{
    qRegisterMetaType <Sensitivity> ("Vch003::Sensitivity");
    qRegisterMetaType <E1Bit> ("Vch003::E1Bit");
    qRegisterMetaType <SsmControlMode> ("Vch003::SsmControlMode");
    qRegisterMetaType <InputSignalState> ("Vch003::InputSignalState");
    qRegisterMetaType <GeneratorError> ("Vch003::GeneratorError");
    qRegisterMetaType <DatabaseTable> ("Vch003::DatabaseTable");
    qRegisterMetaType <DeviceEvent> ("Vch003::DeviceEvent");
}

QString Vch003::e1bitToString(Vch003::E1Bit bit)
{
    switch (bit) {
        case E1Bit::Sa4:     return tr("Sa4");
        case E1Bit::Sa5:     return tr("Sa5");
        case E1Bit::Sa6:     return tr("Sa6");
        case E1Bit::Sa7:     return tr("Sa7");
        case E1Bit::Sa8:     return tr("Sa8");
        default:
            return "";
    }
}

QString Vch003::ssmControlModeToString(Vch003::SsmControlMode mode)
{
    switch (mode) {
        case SsmControlMode::Auto:     return tr("Auto", "ssmmode");
        case SsmControlMode::Manual:   return tr("Manual", "ssmmode");
        default:
            return "";
    }
}

QStringList Vch003::inputSignalStateToStringList(unsigned char state)
{
    QStringList list;
    if(state == 0)
        return list;

    if(state & InputSignalState::StatusDisqualification)        list.append(tr("Disqualification by status"));
    if(state & InputSignalState::LossOfSignal)                  list.append(tr("Loss of signal"));   // "Disqualification by frequency detector"
    //    if(state & InputSignalState::SSMDisqualification)     list.append(tr("Disqualification by SSM"));
    if(state & InputSignalState::RecoveryDelay)                 list.append(tr("Waiting for recovery"));

    if(state & InputSignalState::LossOfFrame)                   list.append(tr("Loss of frame"));
    if(state & InputSignalState::LossOfMultiFrame)              list.append(tr("Loss of multi frame"));

    if(state & InputSignalState::FailureOnTheTransmittingEnd)   list.append(tr("Failure on the transmitting end"));
    if(state & InputSignalState::AbsenceCRC)                    list.append(tr("Absence CRC"));
    if(state & InputSignalState::HDB3Error)                     list.append(tr("HDB3 error"));
    if(state & InputSignalState::NoFrequencyCapture)            list.append(tr("No frequency capture"));
    return list;
}

QStringList Vch003::generatorErrorToStringList(unsigned char error)
{
    QStringList list;
    if(error == 0)
        return list;

    if(error & GeneratorError::GeneratorFailure)            list.append(tr("Generator failure"));
    if(error & GeneratorError::DDSFailure)                  list.append(tr("DDS failure"));
    if(error & GeneratorError::MeasurementSystemFailure)    list.append(tr("Measurement system failure"));
    if(error & GeneratorError::ControlLoopFailure)          list.append(tr("Control loop failure"));
    if(error & GeneratorError::PriorityTableError)          list.append(tr("Priority table assignment error"));
    return list;
}

QVariantList Vch003::eventStatusList()
{
    return { static_cast<int>(SSU::EventStatus::Critical),
                static_cast<int>(SSU::EventStatus::Major),
                static_cast<int>(SSU::EventStatus::Minor),
                static_cast<int>(SSU::EventStatus::Warning)
                //                static_cast<int>(SSU::EventStatus::NotAlarm)      //@N NotAlarm
    };
}

Vch003::Vch003(QObject *parent) : QObject(parent)
{

}
