#ifndef INSTALLMESSAGEHANDLER_H
#define INSTALLMESSAGEHANDLER_H

//DEBUG_REDIRECT_TO_FILE
#include <QDebug>

#ifdef DEBUG_REDIRECT_TO_FILE
namespace ssu {
    void messageOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
}
#endif

#endif // INSTALLMESSAGEHANDLER_H
