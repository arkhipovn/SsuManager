#include "installmessagehandler.h"
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDateTime>
#include <QByteArray>
#include "ssu.h"

#ifdef DEBUG_REDIRECT_TO_FILE
QByteArray typeToString(QtMsgType type)
{
    switch (type) {
        case QtMsgType::QtWarningMsg:
            return "QtWarningMsg";
        case QtMsgType::QtCriticalMsg:
            return "QtCriticalMsg";
        case QtMsgType::QtFatalMsg:
            return "QtFatalMsg";
        case QtMsgType::QtDebugMsg:
            return "QtDebugMsg";
        case QtMsgType::QtInfoMsg:
            return "QtInfoMsg";
        default:
            return "";
    }
}

void debugMessageOutput(const QString &absFilePath, const QString &suffix, QtMsgType type, const QMessageLogContext &context, const QByteArray &msg)
{
    QFileInfo info(absFilePath + "." + suffix);
    QDateTime currentDateTime = QDateTime::currentDateTime();

    if(info.exists()) {
        int m = 0;
        while(info.size() > 10000000) {
            ++m;
            info.setFile(absFilePath + "_" + QString::number(m) + "." + suffix);
        }
    }

    QFile logFile(info.absoluteFilePath());
    switch (type) {
        case QtMsgType::QtWarningMsg:
        case QtMsgType::QtCriticalMsg:
        case QtMsgType::QtFatalMsg:
            if(logFile.open(QIODevice::Append)) {
                logFile.write(typeToString(type) + ": ");
                logFile.write(currentDateTime.toString("dd.MM.yyyy hh:mm:ss:zzz  ").toUtf8());
                logFile.write(msg);
                logFile.write("\n");
                logFile.write("Context: file: ");
                logFile.write(context.file);
                logFile.write(" function: ");
                logFile.write(context.function);
                logFile.write(" line: ");
                logFile.write(QString::number(context.line).toUtf8());
                logFile.write(" category: ");
                logFile.write(context.category);
                logFile.write("\n");
                logFile.close();
            }
            break;
        case QtMsgType::QtDebugMsg:
        case QtMsgType::QtInfoMsg:
        default: {
            if(logFile.open(QIODevice::Append)) {
                logFile.write(currentDateTime.toString("dd.MM.yyyy hh:mm:ss:zzz  ").toUtf8());
                logFile.write(msg);
                logFile.write("\n");
                logFile.close();
            }
            break;
        }
    }
}

void ssu::messageOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = (QString::number(reinterpret_cast<std::uintptr_t>(QThread::currentThread()), 16) + ": " + msg).toLocal8Bit();
    const QString absFilePath = QString(DEBUG_LOG_DIR_PATH + "/" + QDate::currentDate().toString("yyyyMMdd") +"_debug");

    switch (type) {
        case QtMsgType::QtDebugMsg:
        case QtMsgType::QtInfoMsg:
            debugMessageOutput(absFilePath, "log", type, context, localMsg);
            break;
        case QtMsgType::QtWarningMsg:
            debugMessageOutput(absFilePath, "warning", type, context, localMsg);
            debugMessageOutput(absFilePath, "log", type, context, localMsg);
            break;
        case QtMsgType::QtCriticalMsg:
            debugMessageOutput(absFilePath, "critical", type, context, localMsg);
            debugMessageOutput(absFilePath, "log", type, context, localMsg);
            break;
        case QtMsgType::QtFatalMsg:
            debugMessageOutput(absFilePath, "fatal", type, context, localMsg);
            debugMessageOutput(absFilePath, "log", type, context, localMsg);
            abort();
            break;
        default:
            break;
    }
}
#endif

