#include "vch003monitoring.h"
#include "device/configuration/lanconnection.h"
#include "responsehandlers/allresponsehandlers.h"
#include "ssu.h"
#include <QEventLoop>

using namespace ssu;

Vch003Monitoring::Vch003Monitoring(QObject *parent) : QObject(parent),
    connectionSettings_(nullptr),
    reader_(nullptr),
    connectionType_(ConnectionType::Reconnecting)
{
    currentRequest_ = new CurrentRequest(&pollMutex_);

    // Создадим цепочку обязаностей для разбора сообщения авторизации
    auto userBlockedHandler = new UserBlockedHandler(this);
    auto unknownLoginHandler = new UnknownLoginHandler(this);
    auto unknownPasswordHandler = new UnknownPasswordHandler(this);
    auto loginHandler = new LoginHandler(this);
    auto passwordHandler = new PasswordHandler(this);
    auto grettingsMessageHandler = new GrettingsMessageHandler(this);

    userBlockedHandler->setNext(unknownLoginHandler);
    unknownLoginHandler->setNext(unknownPasswordHandler);
    unknownPasswordHandler->setNext(loginHandler);
    loginHandler->setNext(passwordHandler);
    passwordHandler->setNext(grettingsMessageHandler);

    authorizationResponseHandler_ = userBlockedHandler;
    //____________________

    thread_ = new QThread;
    this->moveToThread(thread_);
    QObject::connect(thread_, &QThread::started, this, &Vch003Monitoring::communication);
}

Vch003Monitoring::~Vch003Monitoring()
{
    qInfo() << "~Vch003Monitoring() start";

    disconnectFromDevice();
    if(thread_->isRunning()) {
        QElapsedTimer time; time.start();
        while(!thread_->isFinished() && time.elapsed() < 5000) delay(100);
    }
    delete thread_;
    delete authorizationResponseHandler_;

    qInfo() << "~Vch003Monitoring() end";
}

void Vch003Monitoring::setConnectionSettings(ConnectionSettings *settings)
{
    if(thread_->isRunning())    // уже подключены! Так не должно быть
        return;
    connectionSettings_ = settings;
}

void Vch003Monitoring::clearRequests()
{
    qInfo() << "Vch003Monitoring::clearRequests start";

    currentRequest_->clear();   // Очищаем
    if(pollMutex_.tryLock(SSU::kMutexTimeout)) {
        queue_.clear();
        pollList_.clear();
        commandList_.clear();
        pollMutex_.unlock();
    }

    qInfo() << "Vch003Monitoring::clearRequests end";
}

void Vch003Monitoring::connectToDevice(ConnectionType type)
{
    qDebug() << "Vch003Monitoring::connectToDevice start";

    if(connectionSettings_ == nullptr)
        return;

    connectionType_ = type; // = ConnectionType::OneTimeConnection;
    if(thread_->isRunning())    // уже подключены! Так не должно быть
        return;

    if(setConnectionStatus(ConnectionSettings::Connection))
        thread_->start();   // Запускаем поток. Чтобы подключится

    qDebug() << "Vch003Monitoring::connectToDevice end";
}

void Vch003Monitoring::disconnectFromDevice()
{
    qDebug() << "Vch003Monitoring::disconnectFromDevice start";
    if(!thread_->isRunning())
        return;
    setConnectionStatus(ConnectionSettings::Disconnection); // отключаемся
}

bool Vch003Monitoring::authorization()
{
    qDebug() << "Vch003Monitoring::authorization start";
    if(!thread_->isRunning() || reader_ == nullptr || connectionSettings_ == nullptr)
        return false;
    if(!reader_->isConnected())
        return false;

    Request *request = nullptr;

    switch (connectionStatus()) {
        case ConnectionSettings::WaitingForAuthorization:
            if(!connectionSettings_->username().isEmpty())
                request = new vch003request::SetLogin(connectionSettings_->username());
            break;
        case ConnectionSettings::WaitingForPassword:
            request = new vch003request::SetPassword(connectionSettings_->password());
            break;
        default:
            break;
    }

    bool isok = false;
    if(request != nullptr) {
        if(setConnectionStatus(ConnectionSettings::Authorization))
            isok = send(request);
        else
            delete request;
    }

    qDebug() << "Vch003Monitoring::authorization end";
    return isok;
}

bool Vch003Monitoring::send(Request *request)
{
    if(request == nullptr)
        return false;

    SharedRequest req(request);
    qInfo() << "Vch003Monitoring::send" << req->command() << req->textArg();

    if(connectionSettings_ != nullptr &&
            thread_->isRunning() &&
            reader_->isConnected())
    {
        switch (connectionStatus()) {
            case ConnectionSettings::Connected:
            case ConnectionSettings::ConnectEstablished:
            case ConnectionSettings::WaitingForAuthorization:
            case ConnectionSettings::WaitingForPassword:
            case ConnectionSettings::Authorization:
                if(pollMutex_.tryLock(SSU::kMutexTimeout)) {
                    // Посмотрим отправили ли уже такую команду
                    if(commandList_.size() <= SSU::kMaxNumberOfCommands) {
                        int index = indexOf(commandList_, req.data());

                        if(index == -1) {
                            // Такой нет, глянем, мб ее уже пытаются отправить?
                            index = indexOf(queue_, req.data());

                            if(index != -1 && req->delay() == 0) {
                                // команда есть в списках на отправку. И у текущей команды нет времени ожидания, значит не будем отправлять
                            }
                            else {
                                // Добавляем
                                req->start();   // запоминаем время
                                commandList_.append(req);
                            }
                        }
                        // else Такая команда уже есть в списке.. Нужно удалить ее

                        pollMutex_.unlock();
                        return true;
                    }
                    else {
                        //@N Мб тут стоит удалять первую команду, а новую добавлять..
                        pollMutex_.unlock();
                    }
                }
                break;
            default:
                break;
        }
    }

    auto newCmd = req->execute(false);
    if(newCmd != nullptr) delete newCmd;    // Такого не должно быть, но лучше проверить..
    return false;
}

void Vch003Monitoring::updatePollQueue(const QList<Request *> list)
{
    if(pollMutex_.tryLock(SSU::kMutexTimeout)) {
        qDebug() << "updatePollQueue start";
        queue_.clear();     // очистим очередь команд. Вдруг там лишние будут..

        if(list.isEmpty()) {
            pollList_.clear();
        }
        else if(pollList_.isEmpty()) {
            for(int i = 0; i < list.size(); ++i) {
                pollList_.append(SharedRequest(list.at(i)));
            }
        }
        else {
            // сравним все
            QList <SharedRequest> temp;
            // сравним все
            for(int i = 0; i < list.size(); ++i) {
                int index = indexOf(pollList_, list.at(i));
                if(index == -1) {
                    temp.append(SharedRequest(list.at(i)));
                }
                else {
                    if(list.at(i)->delay() != pollList_.at(index)->delay()) // Если команда также. Изменилось только время опроса
                        pollList_[index]->setDelay(list.at(i)->delay());
                    temp.append(pollList_.at(index));   // вернем старую команду
                    delete list.at(i);                  // удалим новую команду..
                }
            }

            pollList_ = temp;
        }

        for(int i = 0; i < pollList_.size(); ++i)
            pollList_[i]->start();

        pollMutex_.unlock();
        qDebug() << "updatePollQueue end";
    }
}

void Vch003Monitoring::communication()
{
    qInfo() << "start Vch003Monitoring::communication" << QThread::currentThread();

    int counter = 0;
    reader_ = new TcpReader;    //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..

    QObject::connect(reader_, &TcpReader::connected, this, &Vch003Monitoring::clientConnected);
    QObject::connect(reader_, &TcpReader::disconnected, this, &Vch003Monitoring::clientDisconnected);
    QObject::connect(reader_, &TcpReader::dataRead, this, &Vch003Monitoring::clientDataRead);
    QObject::connect(reader_, &TcpReader::fileRead, this, &Vch003Monitoring::clientFileRead);

    int numberOfConAttempts = 0;
    bool running = true;

    while(running && connectionSettings_ != nullptr) {
        auto status = connectionStatus();

        switch (status) {
            case ConnectionSettings::Unconnected:
                running = false;                        // Выходим из цикла
                break;
            case ConnectionSettings::Connection: {
                // Подключаемся к устройству
                int maxNumberOfConnAttempts = connectionType_ == ConnectionType::OneTimeConnection ? SSU::kMaxNumberOfConnectAttempts
                                                                                                   : SSU::kMaxNumberOfReconnectAttempts;
                // будет ограниченное кол-во попыток
                if(maxNumberOfConnAttempts != -1 && ++numberOfConAttempts >= maxNumberOfConnAttempts) {
                    setConnectionStatus(ConnectionSettings::Unconnected);
                }
                else {
                    //@N в будущем если будет несколько способов общения (ком, лан и тд) нужно будет переделать..
                    auto lan = qobject_cast<ssu::LanConnection*>(connectionSettings_);

                    if(lan->hostname().isEmpty()) {   // Больше даже пробовать не будем..
                        setConnectionStatus(ConnectionSettings::Unconnected);
                    }
                    else {
                        if(!reader_->connectToHost(lan->hostname(), lan->port()))   // что-то не так.. Подождем, чтобы еще раз попробовать подключится..
                            delay(SSU::kReconnectInterval);
                    }
                }
            }
                break;

            case ConnectionSettings::ConnectEstablished: {
                if(connectTimer_.hasExpired(SSU::kResponseTime))
                    deviceResponseTimeout();
                delay(SSU::kAuthorisationSendingInterval);
            }
                break;

            case ConnectionSettings::WaitingForAuthorization:
            case ConnectionSettings::WaitingForPassword:
            case ConnectionSettings::Authorization:
            case ConnectionSettings::Connected: {
                qDebug() << "communication case connected start" << reader_;
                auto currentRequestStatus = currentRequest_->status();

                switch (currentRequestStatus) {
                    case CurrentRequest::Status::Timeout: {           // Было отправлено и вышло время ожидания ответа. Ответ не пришел..
                        qDebug() << "communication case connected status Timeout";
                        // Посмотрим сколько было попыток
                        if(currentRequest_->incrementNumberOfAttemptsToSendMessages() >= SSU::kMaxNumberOfAttemptsToSendMessages) {
                            qDebug() << "communication case connected 1#";
                            if(status == ConnectionSettings::Connected) {
                                qInfo() << "Device not responding" << currentRequest_->toString();
                                send(currentRequest_->execute(false));              // Тут точно вернется nullptr, но мало ли.. Зайду в функцию send
                                connectionSettings_->deviceNotResponding();
                            }
                            else {
                                qDebug() << "communication case connected 3#";
                                deviceResponseTimeout();    // Так и не дождались.. выходим..
                            }
                        }
                        else {
                            qDebug() << "communication case connected 4#";
                            // повторим отправку..
                            currentRequest_->send(reader_);
                        }
                        qDebug() << "communication case connected 5#";
                        delay(SSU::kCommandSendingInterval);
                    }
                        break;

                    case CurrentRequest::Status::Null: {            // Еще ничего не отправлено
                        qDebug() << "communication case connected status Null" << reader_->isConnected();
                        if(reader_->isConnected()) {                // И есть соединение с устройством
                            ++counter;  // Чтобы хоть иногда считывать список периодического опроса. Нужно бы подругому сделать это
                            // Нужно найти, что отправить
                            qDebug() << "communication case connected 6#";
                            SharedRequest message = nullptr;
                            if(pollMutex_.tryLock(SSU::kMutexTimeout)) {
                                qDebug() << "communication case connected 7#";
                                // пройдемся и глянем, нужно ли что-то отправить из команд.. У них макс приоритет..
                                for(int i = 0; i < commandList_.size(); ++i) {
                                    auto req = commandList_.at(i);
                                    if(req->hasSend()) {    // Проверим пришло ли время для отправки
                                        enqueue(req);
                                    }
                                }
                                qDebug() << "communication case connected 8#";
                                if(queue_.isEmpty() || counter > 4) {
                                    counter = 0;
                                    // Похоже, что список с командами пуст
                                    // Пройдемся и глянем, нужно ли что-то отправить из периодического опроса..
                                    for(int i = 0; i < pollList_.size(); ++i) {
                                        auto req = pollList_.at(i);
                                        if(req->hasSend()) {        // Проверим пришло ли время для отправки
                                            enqueue(req);
                                        }
                                    }
                                }
                                qDebug() << "communication case connected 9#";
                                // Посмотрим есть ли что отправить
                                if(!queue_.isEmpty()) {
                                    message = queue_.dequeue();
                                    qDebug() << "communication case connected 10#";
                                    // Если это комманда, то нужно удалить сообщение
                                    if(!commandList_.isEmpty()) {
                                        int index = commandList_.indexOf(message);
                                        if(index != -1)
                                            commandList_.removeAt(index);
                                    }
                                }
                                qDebug() << "communication case connected 11#";
                                pollMutex_.unlock();
                            }
                            qDebug() << "communication case connected 12#";
                            if(!message.isNull()) { // Есть что отправить
                                // Нужно удалить сообщение, если это команда
                                currentRequest_->setRequest(message);
                                currentRequest_->send(reader_);
                            }
                            qDebug() << "communication case connected 13#";
                            if(queueQueueIsEmpry()) {   // Сейчас ничего нет. Подождем подольше
                                qDebug() << "communication case connected 14#";
                                delay(SSU::kMonitoringThreadSleep);
                            }
                            else {
                                qDebug() << "communication case connected 15#";
                                delay(SSU::kCommandSendingInterval);
                            }

                            qDebug() << "communication case connected 16#";
                        }
                    }
                        break;

                    case CurrentRequest::Status::Ok:                // Все хорошо. Просто ждем ответ или пока выйдет время ожидания (CurrentRequest::Status::Timeout)
                    case CurrentRequest::Status::Unknown:           // Не понятно.. Просто подождем еще
                    default:
                        delay(SSU::kCommandSendingInterval);
                        break;
                }

                qDebug() << "communication case connected end";
                break;
            }
            case ConnectionSettings::Disconnection: {
                if(reader_->isConnected())
                    reader_->close();   // закрываем все. После закрытия будут очищены все списки и тд
                else {
                    setConnectionStatus(ConnectionSettings::Unconnected);
                    clearRequests(); //нужно очистить запросы самому
                }
            }
                break;
            case ConnectionSettings::Unknown:
                // Непонятно что делать. Просто подождем
            default:
                if(queue_.isEmpty())
                    delay(SSU::kMonitoringThreadSleep);
                else
                    delay(SSU::kCommandSendingInterval);
                break;
        }
    }

    clearRequests(); // нужно очистить запросы. Ну навсякий случай

    thread_->quit();
    delete reader_;

    qInfo() << "end Vch003Monitoring::communication";
}

void Vch003Monitoring::clientConnected()
{
    qInfo() << "Vch003Monitoring::clientConnected";

    // К чему-то подключились
    if(connectionSettings_ == nullptr)
        return;
    connectTimer_.start(); // Нужно дождаться запроса на авторизацию от устройства
    setConnectionStatus(ConnectionSettings::ConnectEstablished);

    qDebug() << "Vch003Monitoring::clientConnected end";
}

void Vch003Monitoring::clientDisconnected()
{
    qInfo() << "Vch003Monitoring::clientDisconnected";

    clearRequests(); //нужно очистить запросы самому

    if(connectionSettings_ == nullptr)
        return;

    auto prevStatus = connectionStatus();
    setConnectionStatus(ConnectionSettings::Unconnected);    // говорим, что отключились..

    // Посмотрим, нужно ли нам переподключится к устройству?
    if(connectionType_ == ConnectionType::Reconnecting &&       // И тип соединения такой
            connectionSettings_->autoConnect() &&               // Разрешено автоподключение
            prevStatus != ConnectionSettings::Disconnection) {    // Мы отключаемся не по своей воли
        setConnectionStatus(ConnectionSettings::Connection); // Переподключаемся
    }

    qDebug() << "Vch003Monitoring::clientDisconnected end";
}

void Vch003Monitoring::clientDataRead(const QString &data)
{
    qDebug() << "Vch003Monitoring::clientDataRead start" << data;
    if(!currentRequest_->isNull()) {
        //@N переделать бы это..
        int returnIntex = data.indexOf(QRegExp("[\\r\\n]+"));   // найдем команду..
        auto cmd = returnIntex == -1 ? data
                                     : data.left(returnIntex);
        if(!cmd.isEmpty() && currentRequest_->toString().compare(cmd, Qt::CaseInsensitive) == 0) {
            qDebug() << "Vch003Monitoring::clientDataRead find ok, execute";
            send(currentRequest_->execute(true));       //@N что-то нужно придумать с этим.. Не всегда же true нужно передавать..
        }
    }

    switch (connectionStatus()) {
        case ConnectionSettings::Connected:
            emit readyRead(data);
            break;
        case ConnectionSettings::ConnectEstablished:
        case ConnectionSettings::WaitingForAuthorization:
        case ConnectionSettings::WaitingForPassword:
        case ConnectionSettings::Authorization:
            if(authorizationResponseHandler_->handle(data)) {
                send(currentRequest_->execute(true));
            }
            break;
        case ConnectionSettings::Unknown:
        default:
            break;
    }

    qDebug() << "Vch003Monitoring::clientDataRead end";
}

void Vch003Monitoring::clientFileRead(const QString &file, bool isok)
{
    qInfo() << "clientFileRead" << file;
    Q_UNUSED(file)
    if(!currentRequest_->isNull()) {
        send(currentRequest_->execute(isok));       // Тут так-то проверку бы сделать.. Точно ли пришло то, что нужно
    }
}

Vch003Monitoring::ConnectionType Vch003Monitoring::connectionType() const
{
    return connectionType_;
}

ConnectionSettings *Vch003Monitoring::connectionSettings() const
{
    return connectionSettings_;
}

void Vch003Monitoring::deviceResponseTimeout()
{
    if(connectionType_ == ConnectionType::OneTimeConnection) {
        qInfo() << "TSSMonitoring::responseTimeout" << "deviceNotResponding";
        emit connectionSettings_->deviceNotResponding();
    }
    // похоже так ничего не получили..
    // разрываем соединение..
    disconnectFromDevice();
}

void Vch003Monitoring::delay(int ms)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()));
    loop.exec();
}

ConnectionSettings::Status Vch003Monitoring::connectionStatus() const
{
    ConnectionSettings::Status status = ConnectionSettings::Status::Unknown;
    if(statusMutex_.tryLock(SSU::kMutexTimeout)) {
        status = connectionSettings_->status();
        statusMutex_.unlock();
    }
    return status;
}

bool Vch003Monitoring::setConnectionStatus(ConnectionSettings::Status status)
{
    if(statusMutex_.tryLock(SSU::kMutexTimeout)) {
        connectionSettings_->setStatus(status);
        statusMutex_.unlock();
        return true;
    }
    return false;
}

bool Vch003Monitoring::queueQueueIsEmpry() const
{
    bool isEmpry = false;
    if(pollMutex_.tryLock(SSU::kMutexTimeout)) {
        isEmpry = queue_.isEmpty();
        pollMutex_.unlock();
    }
    return isEmpry;
}

bool Vch003Monitoring::enqueue(const SharedRequest &request)
{
    if(indexOf(queue_, request.data()) != -1) {
        return false;
    }
    queue_.append(request);
    return true;
}

