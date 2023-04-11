#include "filerequest.h"
#include "ssu.h"

#include <QFile>
#include <QEventLoop>
#include <QTimer>
#include <QCryptographicHash>

#include "vch003/vch003device.h"

namespace ssu {

    GetFileRequest::GetFileRequest(const QString &path, const QString &fileName,
                                   const QString &command, const QString &arg,
                                   const QString &address, Device *device) :
        Request(command, arg, address),
        path_(path),
        fileName_(fileName),
        device_(device)
    {
        setResponseTime(SSU::kFileReadResponseTime);
    }

    Request::SendResult GetFileRequest::send(TcpReader *reader)
    {
        if(device_ != nullptr)
            static_cast<Vch003Device*>(device_)->setFileLoading(true);
        return std::move(Request::send(reader));
    }

    QString GetFileRequest::absoluteFilePath() const
    {
        return path_ + "/" + fileName_;
    }

    Request *GetFileRequest::execute(bool success)
    {
        Q_UNUSED(success)
        if(device_ != nullptr)
            static_cast<Vch003Device*>(device_)->setFileLoading(false);
        return Request::execute(success);
    }

    //____________

    SetFileRequest::SetFileRequest(const QString &path, const QString &fileName,
                                   const QString &command, const QString &arg,
                                   const QString &address, Device *device) :
        Request(command, arg, address),
        path_(path),
        fileName_(fileName),
        device_(device)
    {
        setResponseTime(SSU::kFileReadResponseTime);
    }

    Request::SendResult SetFileRequest::send(TcpReader *reader)
    {
        if(device_ != nullptr)
            static_cast<Vch003Device*>(device_)->setFileLoading(true);

        QFile file(absoluteFilePath());

        bool isok = false;
        QString reqStr;

        if(file.open(QIODevice::ReadOnly)) {
            reader->waitForBytesWritten();
            reqStr = this->toString();

            isok = reader->write(reqStr);
            delay(100);

            if(isok) {
                reader->waitForBytesWritten();
                isok = reader->write("File size " + QString::number(file.size()));
            }

            if(isok) {
                reader->waitForBytesWritten();
                delay(50);
                QCryptographicHash fileHash(QCryptographicHash::Md5);

                while(!file.atEnd() && isok) {
                    QByteArray buffer = file.read(SSU::kMaxMessageSize);
                    fileHash.addData(buffer);
                    isok = reader->writeArray(buffer);
                }
                if(isok) {
                    // Отправим хеш
                    reader->waitForBytesWritten();
                    reader->write("File hash " + fileHash.result().toHex());
                }
            }
            file.close();
        }

        return qMakePair(isok,
                         std::move(reqStr));
    }

    Request *SetFileRequest::execute(bool success)
    {
        Q_UNUSED(success)
        if(device_ != nullptr)
            static_cast<Vch003Device*>(device_)->setFileLoading(false);
        return Request::execute(success);
    }

    QString SetFileRequest::absoluteFilePath() const
    {
        return path_ + "/" + fileName_;
    }

    void SetFileRequest::delay(int ms)
    {
        QEventLoop loop; QTimer::singleShot(ms, &loop, SLOT(quit())); loop.exec();
    }
}
