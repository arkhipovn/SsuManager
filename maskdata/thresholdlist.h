#ifndef THRESHOLDLIST_H
#define THRESHOLDLIST_H

#include "abstractmasklistmodel.h"

namespace ssu {
    class ThresholdList : public AbstractMaskListModel
    {
        Q_OBJECT

    public:
        explicit ThresholdList(QObject *parent = nullptr);

        QVariant data(const QModelIndex &index, int role) const override;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        void append(const MaskData &mask);
        void clear();

        void setName(int index, const QString &name);
        void setMaskData(int index, const DataContainer &data);

        MaskData mask(int index) const override;
        MaskData findMask(const QString &name) const override;

    private:
        QList <MaskData> maskList_;
    };
}

#endif // THRESHOLDLIST_H
