#ifndef VCH003MONITORING_H
#define VCH003MONITORING_H

#include <QThread>
#include <QTimer>
#include <QQueue>

#include "vch003requests.h"
#include "tcpreader.h"
#include "device/configuration/connectionsettings.h"
#include "responsehandlers/responsehandler.h"

#include "currentrequest.h"

namespace ssu {
    //    class CurrentRequest;

    class Vch003Monitoring : public QObject
    {
        Q_OBJECT

    public:
        explicit Vch003Monitoring(QObject *parent = nullptr);
        ~Vch003Monitoring();

        void setConnectionSettings(ssu::ConnectionSettings *settings);

        enum ConnectionType {
            Reconnecting,           // восстановление соединения
            OneTimeConnection       // авторизация в самом начале
        };

        void clearRequests();

        friend class AuthorizationResponseHandler;

        ssu::ConnectionSettings *connectionSettings() const;
        ConnectionType connectionType() const;

    public slots:
        void connectToDevice(ConnectionType type = ConnectionType::OneTimeConnection);
        void disconnectFromDevice();
        bool authorization();

        bool send(Request *request);
        void updatePollQueue(const QList <Request*> = {});

    signals:
        void readyRead(const QString &data);    // получены данные

    private slots:
        void communication();

        void clientConnected();                                 // слот для сигнала connected от тсп клиента
        void clientDataRead(const QString &data);               // пришли данные от тсп клиента
        void clientFileRead(const QString &file, bool isok);    // считали файл
        void clientDisconnected();                              // disconnected от тсп клиента

    private:
        mutable QMutex statusMutex_;        //@N Переносил это в класс ConnectionSettings.. Правильнее бы этому там быть. Но из-за этого все тормозит почему-то.. Разбраться!
        mutable QMutex pollMutex_;

        ResponseHandler *authorizationResponseHandler_;

        ssu::ConnectionSettings *connectionSettings_;
        QThread *thread_;
        TcpReader *reader_;     //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..

        ConnectionType connectionType_;
        QElapsedTimer connectTimer_;   // для ожидания ответа..

        QQueue <SharedRequest> queue_;                  // очередь команд на отправку
        QList <SharedRequest> commandList_;             // список команд, которые нужно отправить в первую очередь и один раз..
        QList <SharedRequest> pollList_;                // список команд для периодической отправки
        CurrentRequest *currentRequest_;                // текущий отправленный запрос

        void deviceResponseTimeout();   // не дождались запроса на авторизацию..

        void delay(int ms);

        ConnectionSettings::Status connectionStatus() const;
        bool setConnectionStatus(ConnectionSettings::Status status);
        bool queueQueueIsEmpry() const;

        template<typename Container>
        int indexOf(const Container &container, const Request * const request) const;    // поиск Request в контейнере
        bool enqueue(const SharedRequest &request);
    };

    template<typename Container>
    int Vch003Monitoring::indexOf(const Container &container, const Request * const request) const
    {
        if(request == nullptr)
            return -1;
        auto first = std::begin(container);
        auto last = std::end(container);
        for(int i = 0; first != last; ++first, ++i) {
            if(*(*first) == *request) {
                return i;
            }
        }
        return -1;
    }
}

#endif // VCH003MONITORING_H
