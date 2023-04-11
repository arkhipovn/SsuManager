#include "tcpreader.h"
#include <QNetworkProxy>

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QRegularExpression>

#include "vch003requests.h"
#include "global.h"

namespace ssu {

    TcpReader::TcpReader(QObject *parent) : QObject(parent),
        fileSize_(-1),
        currentFileSize_(-1)
    {
        currentFileHash_ = new QCryptographicHash(QCryptographicHash::Md5);

        socket_ = new QTcpSocket(this);
        QObject::connect(socket_, &QTcpSocket::readyRead, this, &TcpReader::readyRead);

        QObject::connect(socket_, &QTcpSocket::connected, this, &TcpReader::connected);
        QObject::connect(socket_, &QTcpSocket::disconnected, this, &TcpReader::disconnected);

        //    QObject::connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), SIGNAL(error(QAbstractSocket::SocketError)));
        //    QObject::connect(socket_, &QTcpSocket::stateChanged, this, &TcpReader::stateChanged);
    }

    TcpReader::~TcpReader()
    {
        qInfo() << "~TcpReader() start";
        delete currentFileHash_;
        qInfo() << "~TcpReader() end";
    }

    bool TcpReader::isConnected() const
    {
        return socket_->state() == QAbstractSocket::ConnectedState;
    }

    bool TcpReader::write(const QString &array)
    {
        qInfo() << QDateTime::currentDateTime() << "TcpReader::write. Socket state:" << socket_->state() << "text:" << (array.toLatin1());

        if(array.isEmpty())
            return false;
        if(socket_->state() != QAbstractSocket::ConnectedState) return false;
        return socket_->write(array.toLatin1() + "\r\n");
    }

    bool TcpReader::writeArray(const QByteArray &array)
    {
        qInfo() << QDateTime::currentDateTime() << "TcpReader::write array" << array;
        if(socket_->state() != QAbstractSocket::ConnectedState) return false;
        return socket_->write(array);
    }

    bool TcpReader::connectToHost(const QString &addr, quint16 port)
    {
        qInfo() << QDateTime::currentDateTime() << "TcpReader::connectToHost" << addr << port;

        if(socket_->state() == QAbstractSocket::ConnectedState) return true;

        socket_->setProxy(QNetworkProxy::NoProxy);
        socket_->connectToHost(addr, port);
        if(!socket_->waitForConnected(1000)) return false;
        return true;
    }

    void TcpReader::close()
    {
        qInfo() << QDateTime::currentDateTime() << "TcpReader::close";

        if(socket_->state() == QAbstractSocket::ConnectedState)
            socket_->close();
    }

    bool TcpReader::waitForBytesWritten(int msecs)
    {
        if(socket_->state() != QAbstractSocket::ConnectedState) return false;
        return socket_->waitForBytesWritten(msecs);
    }

    void TcpReader::readyRead()
    {
        QByteArray array = socket_->readAll();

        if(fileSize_ != -1) {
            // считываем файл похоже..
            readFileFromDevice(array);
        }
        else {
            // посмотрим есть ли в строке начало считывания файла
            int iFileSize = array.indexOf(BACKUP_REQUEST_MESSAGE);

            if(iFileSize == -1) readDataFromDevice(array);  // не передача файла, значит посмотрим остальное..
            else {
                // Сейчас начнет приходить файл
                // Вырежем все, что пришло до iFileSize и отправим на обработку. Чтобы завершить начатое..

                auto lastData = iFileSize == 0 ? "" : array.left(iFileSize);
                readDataFromDevice(lastData);
                // Обработали. Если что-то осталось, то это уже точно не придется..
                data_.clear();
                if(iFileSize != 0)
                    array = array.mid(iFileSize);   // вырежем все что было до этого

                bool isok;
                currentFileHash_->reset();
                currentFileSize_ = 0;
                fileSize_ = -1;

                int returnSymbol = array.indexOf("\r\n");
                if(returnSymbol != -1) {
                    // Теперь найдет сколько байт должны мы принять
                    auto aFileSize = array.mid(BACKUP_REQUEST_MESSAGE.size() + 1, returnSymbol - BACKUP_REQUEST_MESSAGE.size() - 1);
                    fileSize_ = aFileSize.toLongLong(&isok);
                    if(isok) {
                        // посмотрим существует ли путь до файла
                        QDir d(TEMP_DIR_PATH);
                        if(!d.exists()) d.mkpath(TEMP_DIR_PATH);

                        // Если файл уже есть, то удалим сначала
                        isok = d.remove(SSU::downloadTempFile());
                        // найдем файл куда можно записать данные
                        //                    QString file = SSU::backupTempFile();
                        //                    int i = 0;
                        //                    while(QFile::exists(file + (i > 0 ? "_" + QString::number(i) : ""))) { ++i; }   // уже ест
                        //                    fileName_ = file + (i > 0 ? "_" + QString::number(i) : "");
                    }

                    array = array.mid(returnSymbol + 2);
                    auto leftovers = readFileFromDevice(array);
                    // Если что-то осталось, то значит новое сообщение пошло..
                    if(!leftovers.isEmpty()) readDataFromDevice(leftovers);
                }
            }
        }
    }

    void TcpReader::readDataFromDevice(QByteArray &array)
    {
        if(array.isEmpty() && data_.isEmpty()) return;

        QString line = QString::fromLocal8Bit(array);     //QString::fromStdString(arr.toStdString());

        static QString reStr = GREETING_MESSAGE + "|" + LOGIN_REQUEST_MESSAGE + "|" + PASSWORD_REQUEST_MESSAGE;
        QRegularExpression re(reStr);

        int offset = 0;
        while(offset < line.size()) {
            auto match = re.match(line, offset);
            if(offset == match.capturedEnd()) break;    // перестраховка..

            if(!match.hasMatch()) {
                // ничего не нашли..
                data_ += line.mid(offset);
                break;
            }
            // что-то нашли..

            if(!data_.isEmpty() || match.capturedStart() != offset) {
                // что-то было до этого..
                if(match.capturedStart() != offset) data_ += line.mid(offset, match.capturedStart() - offset);

                if(!data_.isEmpty()) {
                    if(!data_.isEmpty()) {
                        emit dataRead(data_);
                        data_.clear();
                    }
                }
            }

            emit dataRead(match.captured());    // отправляем сам разделитель, что пришел..
            offset = match.capturedEnd();
        }
    }

    QByteArray TcpReader::readFileFromDevice(QByteArray &array)
    {
        if(array.isEmpty()) return "";

        int iFileHash = array.indexOf(BACKUP_HASH_MESSAGE);

        QByteArray out;
        if(iFileHash == -1) {
            // конец еще не пришел. Просто записываем в файл..
            QFile file(SSU::downloadTempFile());
            if(file.open(QIODevice::Append)) {
                currentFileSize_ += array.size();
                currentFileHash_->addData(array);
                file.write(array);
                file.close();
            }
        }
        else {
            // В данных есть конец считывания файла. Сначала допишем в файл все до конца..
            auto fileData = array.left(iFileHash);
            if(!fileData.isEmpty()) {
                // запишем в файл
                QFile file(SSU::downloadTempFile());
                if(file.open(QIODevice::Append)) {
                    currentFileSize_ += fileData.size();
                    currentFileHash_->addData(fileData);

                    file.write(fileData);
                    file.close();
                }
            }

            // Теперь считаем хеш
            int returnSymbol = array.indexOf("\r\n", iFileHash);
            if(returnSymbol != -1) {
                auto fileHash = array.mid(iFileHash + BACKUP_HASH_MESSAGE.size() + 1, returnSymbol - iFileHash - BACKUP_HASH_MESSAGE.size() - 1);
                emit fileRead(SSU::downloadTempFile(), (currentFileSize_ == fileSize_ && currentFileHash_->result().toHex() == fileHash));
            }

            out = array.mid(returnSymbol);  // +2

            currentFileHash_->reset();
            currentFileSize_ = 0;
            fileSize_ = -1;
        }

        return out;
    }

}

