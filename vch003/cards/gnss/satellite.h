#ifndef SATELLITE_H
#define SATELLITE_H

namespace ssu {
    struct Satellite
    {
        int type;
        int id;
        double elevation;
        double azimuth;
        double snr;             // Отношение сигнал/шум
        bool tracked;
    };
}

#endif // SATELLITE_H
