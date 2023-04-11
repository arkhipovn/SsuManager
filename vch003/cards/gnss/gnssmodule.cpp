#include "gnssmodule.h"
#include <QDebug>

namespace ssu {

    GnssModule::GnssModule(QObject *parent) : QObject(parent),
        currentDateTime_(QDateTime::currentDateTimeUtc()),
        latitude_({ SSU::Hemisphere::North, 0 }),
        longitude_({ SSU::Hemisphere::West, 0 }),
        altitude_(0),
        nmeasDataValidity_(true),
        pdop_(0),
        pdopMask_(0.1),
        elevationMask_(0),
        cableDelay_(0),
        snr_(0),
        snrMask_(0),
        trackingSatellites_(0)
    {

    }

    double GnssModule::latitude() const
    {
        return latitude_.value;
    }

    void GnssModule::setLatitude(const Location &latitude)
    {
        if(latitude_ == latitude)
            return;
        latitude_ = latitude;
        emit latitudeChanged();
    }

    double GnssModule::longitude() const
    {
        return longitude_.value;
    }

    void GnssModule::setLongitude(const Location &longitude)
    {
        if(longitude_ == longitude)
            return;
        longitude_ = longitude;
        emit longitudeChanged();
    }

    double GnssModule::altitude() const
    {
        return altitude_;
    }

    void GnssModule::setAltitude(double altitude)
    {
        if(!qFuzzyCompare(altitude_, altitude)) {
            altitude_ = altitude;
            emit altitudeChanged();
        }
    }

    SSU::Hemisphere GnssModule::latitudeHemisphere() const
    {
        return latitude_.hemisphere;
    }

    SSU::Hemisphere GnssModule::longitudeHemisphere() const
    {
        return longitude_.hemisphere;
    }

    bool GnssModule::nmeasDataValidity() const
    {
        return nmeasDataValidity_;
    }

    void GnssModule::setNmeasDataValidity(bool nmeasDataValidity)
    {
        if(nmeasDataValidity_ != nmeasDataValidity) {
            nmeasDataValidity_ = nmeasDataValidity;
            nmeasDataValidityChanged();
        }
    }

    double GnssModule::pdopMask() const
    {
        return pdopMask_;
    }

    void GnssModule::setPdopMask(double pdopMask)
    {
        if(!qFuzzyCompare(pdopMask_, pdopMask)) {
            pdopMask_ = pdopMask;
            pdopMaskChanged();
        }
    }

    int GnssModule::elevationMask() const
    {
        return elevationMask_;
    }

    void GnssModule::setElevationMask(int elevationMask)
    {
        if(elevationMask_ != elevationMask) {
            elevationMask_ = elevationMask;
            elevationMaskChanged();
        }
    }

    int GnssModule::cableDelay() const
    {
        return cableDelay_;
    }

    void GnssModule::setCableDelay(int cableDelay)
    {
        if(cableDelay_ != cableDelay) {
            cableDelay_ = cableDelay;
            cableDelayChanged();
        }
    }

    int GnssModule::snr() const
    {
        return snr_;
    }

    void GnssModule::setSnr(int snr)
    {
        if(snr_ != snr) {
            snr_ = snr;
            emit snrChanged();
        }
    }

    int GnssModule::snrMask() const
    {
        return snrMask_;
    }

    void GnssModule::setSnrMask(int snrMask)
    {
        if(snrMask_ != snrMask) {
            snrMask_ = snrMask;
            emit snrMaskChanged();
        }
    }

    double GnssModule::pdop() const
    {
        return pdop_;
    }

    void GnssModule::setPdop(double pdop)
    {
        if(!qFuzzyCompare(pdop_, pdop)) {
            pdop_ = pdop;
            pdopChanged();
        }
    }

    int GnssModule::trackingSatellites() const
    {
        return trackingSatellites_;
    }

    void GnssModule::setTrackingSatellites(int trackingSatellites)
    {
        if(trackingSatellites_ != trackingSatellites) {
            trackingSatellites_ = trackingSatellites;
            emit trackingSatellitesChanged();
        }
    }

    int GnssModule::numberOfVisibleSat(int systemType) const
    {
        auto it = numberOfVisibleSat_.constFind(systemType);
        return it == numberOfVisibleSat_.constEnd() ? 0
                                                    : it.value();
    }

    void GnssModule::setNumberOfVisibleSat(int systemType, int count)
    {
        auto it = numberOfVisibleSat_.find(systemType);
        if(it == numberOfVisibleSat_.end()) {
            numberOfVisibleSat_.insert(systemType, count);
            emit numberOfVisibleSatChanged();
        }
        else {
            if(it.value() != count) {
                it.value() = count;
                emit numberOfVisibleSatChanged();
            }
        }
    }

    int GnssModule::numberOfVisibleGpsSat() const
    {
        return numberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::GPS));
    }

    int GnssModule::numberOfVisibleGlonassSat() const
    {
        return numberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::GLONASS));
    }

    int GnssModule::numberOfVisibleBeiDouSat() const
    {
        return numberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::BeiDou));
    }

    int GnssModule::numberOfVisibleGalileoSat() const
    {
        return numberOfVisibleSat(static_cast<int>(SSU::SatelliteSystem::Galileo));
    }

    QList<int> GnssModule::selectedSystems() const
    {
        return selectedSystems_;
    }

    void GnssModule::setSelectedSystems(const QList<int> &selectedSystems)
    {
        if(selectedSystems_ != selectedSystems) {
            selectedSystems_ = selectedSystems;
            emit selectedSystemsChanged();
        }
    }

    bool GnssModule::isSystemSelected(int system)
    {
        return selectedSystems_.contains(system);
    }
    
    QDateTime GnssModule::currentDateTime() const
    {
        return currentDateTime_;
    }
    
    void GnssModule::setCurrentDateTime(const QDateTime &currentDateTime)
    {
        if(!currentDateTime.isValid()) return;
        if(currentDateTime_ != currentDateTime) {
            currentDateTime_ = currentDateTime;
            emit currentDateTimeChanged();
        }
    }
    
    bool Location::operator ==(const Location &other) const
    {
        if(other.hemisphere != hemisphere)
            return false;
        return qFuzzyCompare(value, other.value);
    }

}
