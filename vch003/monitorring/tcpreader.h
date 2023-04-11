#ifndef TCPREADER_H
#define TCPREADER_H

#include <QTcpSocket>
#include <QCryptographicHash>

namespace ssu {
    class TcpReader : public QObject
    {
        Q_OBJECT

    public:
        explicit TcpReader(QObject *parent = nullptr);
        ~TcpReader();

        bool isConnected() const;

    public slots:
        bool connectToHost(const QString &addr, quint16 port = 5000);
        void close();

        bool waitForBytesWritten(int msecs = 30000);
        bool write(const QString &array);
        bool writeArray(const QByteArray &array);

    private slots:
        void readyRead();

    signals:
        void finished();    // завершение потока

        void connected();       // подключились к девайсу
        void disconnected();    // отключились от девайса

        void dataRead(const QString &data);
        void fileRead(const QString &file, bool isok);

    private:
        QTcpSocket *socket_;
        QString data_;

        // read file
        qlonglong fileSize_;                 // какой должен быть размер

        qlonglong currentFileSize_;          // сколько сейчас уже есть..
        QCryptographicHash *currentFileHash_;


        void readDataFromDevice(QByteArray &array);
        QByteArray readFileFromDevice(QByteArray &array);   // вернет лишнее. Если что-то осталось..
    };
}

#endif // TCPREADER_H
