#ifndef PROGRAMDATAMANAGEMENTMODEL_H
#define PROGRAMDATAMANAGEMENTMODEL_H

#include "models/filelistmodel.h"

namespace ssu {
    class ProgramDataManagementModel : public FileListModel
    {
        Q_OBJECT

    public:
        explicit ProgramDataManagementModel(QObject *parent = nullptr);
        ~ProgramDataManagementModel();

    public slots:
        void update() override;

    protected:
        void timerEvent(QTimerEvent *event) override;

    private:
        int timerId_;             // для таймера

        void start();
        void stop();

        void loadFileListFromPath() override;
    };
}

#endif // PROGRAMDATAMANAGEMENTMODEL_H
