#include "maskrecommendation.h"
#include <math.h>

ssu::MaskRecommendation::MaskRecommendation(const QString &name) : name_(name)
{

}

QString ssu::MaskRecommendation::name() const
{
    return name_;
}

QList<double> ssu::MaskRecommendation::mtie(const QList<int> &intervals) const
{
    QList <double> list;
    for(auto it = intervals.constBegin(); it != intervals.constEnd(); ++it)
        list.append(mtie(*it));
    return list;
}

QList<double> ssu::MaskRecommendation::tdev(const QList<int> &intervals) const
{
    QList <double> list;
    for(auto it = intervals.constBegin(); it != intervals.constEnd(); ++it)
        list.append(tdev(*it));
    return list;
}

//------------------------------------------------------

ssu::G811::G811() : MaskRecommendation("G811") {}

double ssu::G811::mtie(double interval) const
{
    return (interval > 0.1 && interval <= 100) ? 3e-9
                                               : (interval > 100 && interval <= 1000) ? 0.03e-9
                                                                                      : (interval > 1000 && interval <= 10000) ? 30e-9
                                                                                                                               : 0;
}

double ssu::G811::tdev(double interval) const
{
    if(interval > 1000)
        return (1.e-5 * interval + 0.29) * 1e-6;
    if(interval > 0.1)
        return (0.275 * 1.e-3 * interval + 0.025) * 1.e-6;
    return 0;
}


//------------------------------------------------------


ssu::G811d1::G811d1() : MaskRecommendation("G811.1") {}

double ssu::G811d1::mtie(double interval) const
{
    if(interval > 1000)
        return (1e-6 * interval + 0.0140375) * 1e-6;
    if(interval > 100)
        return (0.0375 * 1e-6 * interval + 0.015) * 1.e-6;
    if(interval > 1)
        return (0.11114 * 1.e-3 * interval + 0.00389) * 1.e-6;
    if(interval > 0.1)
        return 0.004e-6;
    return 0;
}

double ssu::G811d1::tdev(double interval) const
{
    return (interval > 0.1 && interval <= 10000) ? 1.e-9
                                                 : 0;
}
//------------------------------------------------------


ssu::G812Tolerance::G812Tolerance() : MaskRecommendation("G812. Wander tolerance") {}

double ssu::G812Tolerance::mtie(double interval) const
{
    if(interval > 0.1 && interval <= 7.5) return 0.75e-6;
    if(interval > 7.5 && interval <= 20) return 0.1e-6 * interval;
    if(interval > 20 && interval <= 400) return 2e-6;
    if(interval > 400 && interval <= 1000 ) return 0.005e-6 * interval;
    if(interval > 1000 && interval <= 10000 ) return 5e-6;
    return 0;
}

double ssu::G812Tolerance::tdev(double interval) const
{
    if(interval > 0.05 && interval <= 280) return (0.3 + 0.0025 * interval) * 1e-6;
    if(interval > 280) return (0.997 + 0.00001 * interval) * 1e-6;
    return 0;
}

//------------------------------------------------------

ssu::G812LockedMode::G812LockedMode() : MaskRecommendation("G812. Wander in locked mode") {}

double ssu::G812LockedMode::mtie(double interval) const
{
    return (interval > 0.1 && interval <= 9) ? 24.e-9
                                             : (interval > 9 && interval <= 400) ? 8 * std::sqrt(interval) * 1.e-9
                                                                                 : (interval > 400 && interval <= 10000) ? 160.e-9
                                                                                                                         : 0;
}

double ssu::G812LockedMode::tdev(double interval) const
{
    return (interval > 0.1 && interval <= 25) ? 3.e-9
                                              : (interval > 25 && interval <= 100) ? 0.12 * interval * 1.e-9
                                                                                   : (interval > 100 && interval <= 10000) ? 12.e-9
                                                                                                                           : 0;
}
