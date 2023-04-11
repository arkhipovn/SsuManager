#include "userrights.h"

ssu::UserRights::UserRights(QObject *parent) : QObject(parent)
{

}

bool ssu::UserRights::parameterControl(SSU::UserLevel level) const
{
    switch (level) {
        case SSU::UserLevel::Admin:
        case SSU::UserLevel::Security:
            return true;
        case SSU::UserLevel::User:
        default: break;
    }
    return false;
}

bool ssu::UserRights::networkControl(SSU::UserLevel level) const
{
    switch (level) {
        case SSU::UserLevel::Security:
            return true;
        case SSU::UserLevel::Admin:
        case SSU::UserLevel::User:
        default: break;
    }
    return false;
}

bool ssu::UserRights::deviceControl(SSU::UserLevel level) const
{
    switch (level) {
        case SSU::UserLevel::Security:
            return true;
        case SSU::UserLevel::Admin:
        case SSU::UserLevel::User:
        default: break;
    }
    return false;
}

bool ssu::UserRights::eventControl(SSU::UserLevel level) const
{
    switch (level) {
        case SSU::UserLevel::Admin:
        case SSU::UserLevel::Security:
            return true;
        case SSU::UserLevel::User:
        default: break;
    }
    return false;
}
