#ifndef GNSSMODULE_H
#define GNSSMODULE_H

#include <QObject>
#include <QDateTime>
#include "ssu.h"

namespace ssu {
    struct Location {
        SSU::Hemisphere hemisphere;
        double value;
        bool operator ==(const Location &other) const;
    };

    class GnssModule : public QObject
    {
        Q_OBJECT

        Q_PROPERTY(double latitude READ latitude NOTIFY latitudeChanged)
        Q_PROPERTY(SSU::Hemisphere latitudeHemisphere READ latitudeHemisphere NOTIFY latitudeChanged)

        Q_PROPERTY(double longitude READ longitude NOTIFY longitudeChanged)
        Q_PROPERTY(SSU::Hemisphere longitudeHemisphere READ longitudeHemisphere NOTIFY longitudeChanged)

        Q_PROPERTY(double altitude READ altitude NOTIFY altitudeChanged)

        Q_PROPERTY(double pdop READ pdop NOTIFY pdopChanged)
        Q_PROPERTY(double pdopMask READ pdopMask NOTIFY pdopMaskChanged)
        Q_PROPERTY(int elevationMask READ elevationMask NOTIFY elevationMaskChanged)
        Q_PROPERTY(int cableDelay READ cableDelay NOTIFY cableDelayChanged)

        Q_PROPERTY(bool nmeasDataValidity READ nmeasDataValidity NOTIFY nmeasDataValidityChanged)

        Q_PROPERTY(int snr READ snr NOTIFY snrChanged)
        Q_PROPERTY(int snrMask READ snrMask NOTIFY snrMaskChanged)

        Q_PROPERTY(int trackingSatellites READ trackingSatellites NOTIFY trackingSatellitesChanged)

        Q_PROPERTY(int numberOfVisibleGpsSat READ numberOfVisibleGpsSat NOTIFY numberOfVisibleSatChanged)
        Q_PROPERTY(int numberOfVisibleGlonassSat READ numberOfVisibleGlonassSat NOTIFY numberOfVisibleSatChanged)
        Q_PROPERTY(int numberOfVisibleBeiDouSat READ numberOfVisibleBeiDouSat NOTIFY numberOfVisibleSatChanged)
        Q_PROPERTY(int numberOfVisibleGalileoSat READ numberOfVisibleGalileoSat NOTIFY numberOfVisibleSatChanged)

        Q_PROPERTY(QList <int> selectedSystems READ selectedSystems NOTIFY selectedSystemsChanged)

        Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)

    public:
        explicit GnssModule(QObject *parent = nullptr);

        double latitude() const;
        void setLatitude(const Location &latitude);

        double longitude() const;
        void setLongitude(const Location &longitude);

        double altitude() const;
        void setAltitude(double altitude);

        SSU::Hemisphere latitudeHemisphere() const;
        SSU::Hemisphere longitudeHemisphere() const;

        bool nmeasDataValidity() const;
        void setNmeasDataValidity(bool nmeasDataValidity);

        double pdopMask() const;
        void setPdopMask(double pdopMask);

        int elevationMask() const;
        void setElevationMask(int elevationMask);

        int cableDelay() const;
        void setCableDelay(int cableDelay);

        int snr() const;
        void setSnr(int snr);

        int snrMask() const;
        void setSnrMask(int snrMask);

        double pdop() const;
        void setPdop(double pdop);

        int trackingSatellites() const;
        void setTrackingSatellites(int trackingSatellites);

        int numberOfVisibleSat(int systemType) const;
        void setNumberOfVisibleSat(int systemType, int count);

        int numberOfVisibleGpsSat() const;
        int numberOfVisibleGlonassSat() const;
        int numberOfVisibleBeiDouSat() const;
        int numberOfVisibleGalileoSat() const;

        QList<int> selectedSystems() const;
        void setSelectedSystems(const QList<int> &selectedSystems);

        Q_INVOKABLE bool isSystemSelected(int system);

        QDateTime currentDateTime() const;
        void setCurrentDateTime(const QDateTime &currentDateTime);

    signals:
        void latitudeChanged();
        void longitudeChanged();
        void altitudeChanged();

        void nmeasDataValidityChanged();

        void pdopChanged();
        void pdopMaskChanged();
        void elevationMaskChanged();
        void cableDelayChanged();

        void snrChanged();
        void snrMaskChanged();

        void trackingSatellitesChanged();
        void numberOfVisibleSatChanged();

        void selectedSystemsChanged();
        void currentDateTimeChanged();

    private:
        QDateTime currentDateTime_;

        Location latitude_;             // Широта
        Location longitude_;            // Долгота
        double altitude_;               // Высота антенны над уровнем моря

        bool nmeasDataValidity_;		// Данные верны/не верны

        double pdop_;
        double pdopMask_;               // Маска коэфф PDOP
        int elevationMask_;             // градусы
        int cableDelay_;                // нс

        int snr_;                       // Минимальное количество спутников с маской SNR
        int snrMask_;                   // Маска порога SNR

        int trackingSatellites_;        // Кол-во отслеживаемых спутников

        QHash <int, int> numberOfVisibleSat_;    // Кол-во видимых спутников по каждому типу
        QList <int> selectedSystems_;
    };
}

#endif // GNSSMODULE_H
