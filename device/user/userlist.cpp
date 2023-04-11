#include "userlist.h"
#include <QDateTime>
#include <QDebug>

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextStream>
#include <QPainter>

#include "csettings.h"
#include "src/QCC/qccutils.h"

ssu::UserList::UserList(QObject *parent) : QAbstractListModel(parent)
{

}

void ssu::UserList::set(const QVector<User> &users)
{
    clear();
    if(users.isEmpty()) return;
    beginInsertRows(QModelIndex(), 0, users.size() - 1);
    this->users = users;
    endInsertRows();
}

void ssu::UserList::clear()
{
    if(users.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, users.size() - 1);
    users.clear();
    endRemoveRows();
}

QHash<int, QByteArray> ssu::UserList::roleNames() const
{
    static const QHash <int, QByteArray> roles = QHash <int, QByteArray> {
        { Role::NameRole, "name" },
        { Role::LevelRole, "level" },
        { Role::ConnectedRole, "connected" },
        { Role::UntilTimeRole, "untilTime" }
    };
    return roles;
}

int ssu::UserList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return users.size();
}

QVariant ssu::UserList::data(const QModelIndex &index, int role) const
{
    if(index.row() >= users.size()) return QVariant();
    switch (role) {
        case Role::NameRole: return users.at(index.row()).name;
        case Role::LevelRole: return static_cast<int>(users.at(index.row()).level);
        case Role::ConnectedRole: return users.at(index.row()).connected;
        case Role::UntilTimeRole:
            return users.at(index.row()).untilTime == 0 ? QVariant("")
                                                        : QCCUtils::qmlDateTime(QDateTime::fromSecsSinceEpoch(users.at(index.row()).untilTime, Qt::UTC),
                                                                                CSettings::getInstance()->timeSpec()).toLocalTime();
        default: break;
    }
    return QVariant();
}

void ssu::UserList::saveInventoryInformation()
{
    QString filters = "*.csv;";
    QString defaultFilter("*.csv");

    QString name = QFileDialog::getSaveFileName(0, tr("Save file"), QDir::currentPath(),
                                                filters, &defaultFilter);
    if(name.isEmpty()) return;

    QFile f(name);
    if(f.open(QIODevice::WriteOnly)) {
        QTextStream ts(&f);

        QStringList header = { tr("Username"), tr("Level"), tr("Connected") };
        for(auto h : header) ts << h << ";";
        ts << '\n';

        for(int i = 0; i < users.size(); ++i) {
            ts << users.at(i).name + ";"
               << SSU::userLevelToString(users.at(i).level) + ";"
               << (users.at(i).connected ? "true" : "false") + '\n';
        }

        f.close();
    }
}

void ssu::UserList::printInventoryInformation()
{
    QStringList header = { tr("Username"), tr("Level"), tr("Connected") };
    int indent = 30;

    QString text;
    QTextStream ts(&text);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    for(auto h : header) {
        ts << qSetFieldWidth(indent) << left << h;
    }

    ts << qSetFieldWidth(1) << endl;

    for(int i = 0; i < users.size(); ++i) {
        ts << qSetFieldWidth(indent) << left << users.at(i).name;
        ts << qSetFieldWidth(indent) << left << SSU::userLevelToString(users.at(i).level);
        ts << qSetFieldWidth(indent) << left << (users.at(i).connected ? "true" : "false");
        ts << qSetFieldWidth(1) << endl;
    }
#else
    for(auto h : header) {
        ts << qSetFieldWidth(indent) << Qt::left << h;
    }

    ts << qSetFieldWidth(1) << Qt::endl;

    for(int i = 0; i < users.size(); ++i) {
        ts << qSetFieldWidth(indent) << Qt::left << users.at(i).name;
        ts << qSetFieldWidth(indent) << Qt::left << SSU::userLevelToString(users.at(i).level);
        ts << qSetFieldWidth(indent) << Qt::left << (users.at(i).connected ? "true" : "false");
        ts << qSetFieldWidth(1) << Qt::endl;
    }
#endif
    QPrinter printer;
    QPrintDialog printDialog(&printer);
    printDialog.setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); //@N
    printDialog.setWindowTitle("Print Document");
    auto exec = printDialog.exec();
    if(exec == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.drawText(painter.viewport(), Qt::AlignLeft|Qt::AlignTop, text.toLatin1());
        painter.end();
    }
}

