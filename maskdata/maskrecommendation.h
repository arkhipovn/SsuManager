#ifndef MASKRECOMMENDATION_H
#define MASKRECOMMENDATION_H

#include <QList>

namespace ssu {
    class MaskRecommendation
    {
    public:
        MaskRecommendation(const QString &name);
        virtual ~MaskRecommendation() = default;

        QString name() const;

        virtual double mtie(double interval) const = 0;
        virtual double tdev(double interval) const = 0;

        QList <double> mtie(const QList <int> &intervals) const;
        QList <double> tdev(const QList <int> &intervals) const;

    protected:
        const QString name_;
    };


    //------------------------------------------------------

    class G811 : public MaskRecommendation
    {
    public:
        G811();
        double mtie(double interval) const override;
        double tdev(double interval) const override;
    };

    //------------------------------------------------------

    class G811d1 : public MaskRecommendation
    {
    public:
        G811d1();
        double mtie(double interval) const override;
        double tdev(double interval) const override;
    };

    //------------------------------------------------------

    class G812Tolerance : public MaskRecommendation
    {
    public:
        G812Tolerance();
        double mtie(double interval) const override;
        double tdev(double interval) const override;
    };

    //------------------------------------------------------

    class G812LockedMode : public MaskRecommendation
    {
    public:
        G812LockedMode();
        double mtie(double interval) const override;
        double tdev(double interval) const override;
    };
}

#endif // MASKRECOMMENDATION_H
