#include "panel.h"

#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextStream>
#include <QPainter>

ssu::Panel::Panel(const QMap<int, ssu::SlotConfiguration> &panelConfiguration, QObject *parent) : QObject(parent)
{
    panelConfiguration_ = new SsuPanelConfiguration(panelConfiguration, this);
    cardFactory_ = new CardFactory;
    inventory_ = new Inventory(cardFactory_, this);
}

ssu::Panel::~Panel()
{
    delete cardFactory_;
}

ssu::Inventory *ssu::Panel::inventory() const
{
    return inventory_;
}

QString ssu::Panel::serialNumber() const
{
    return serialNumber_;
}

void ssu::Panel::setSerialNumber(const QString &serialNumber)
{
    if(serialNumber_ != serialNumber) {
        serialNumber_ = serialNumber;
        emit serialNumberChanged();
    }
}

ssu::SsuPanelConfiguration *ssu::Panel::panelConfiguration() const
{
    return panelConfiguration_;
}

void ssu::Panel::saveInventoryInformation()
{
    QString filters = "*.csv;";
    QString defaultFilter("*.csv");

    QString name = QFileDialog::getSaveFileName(0, tr("Save file"), QDir::currentPath(),
                                                filters, &defaultFilter);
    if(name.isEmpty()) return;

    QFile f(name);
    if(f.open(QIODevice::WriteOnly)) {
        QTextStream ts(&f);

        ts << tr("Panel serial number") << ";" << serialNumber_ << '\n';

        //        auto card = qobject_cast <MonitoringCard*> (cardList->getCard(SSU::Monitoring));
        //        if(card != nullptr) {
        //            ts << "Power card A" << ";" << card->getPowerA()->getSerialNumber() << '\n';
        //            ts << "Power card B" << ";" << card->getPowerB()->getSerialNumber() << '\n';
        //        }

        QStringList header = { tr("Address"), tr("Card"), tr("Serial number"), tr("Software"), tr("Hardware") };
        for(auto h : header) ts << h << ";";
        ts << '\n';
        for(int i = 0; i < inventory_->size(); ++i) {
            auto card = inventory_->at(i);
            if(card->type() == SSU::Stub)
                continue;
            ts << card->slot() + ";"
               << SSU::cardTypeToString(static_cast<SSU::CardType>(card->type())) + ";"
               << card->serialNumber() + ";"
               << card->software() + ";"
               << card->hardware() + '\n';
        }

        f.close();
    }
}

void ssu::Panel::printInventoryInformation()
{
    QStringList header = { tr("Address"), tr("Card"), tr("Serial number"), tr("Software"), tr("Hardware") };
    int indent = 30;

    QString text;
    QTextStream ts(&text);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    ts << tr("Panel serial number") + ": " << serialNumber_ << endl;

    for(auto h : header) {
        ts << qSetFieldWidth(indent) << left << h;
    }

    ts << qSetFieldWidth(1) << endl;

    for(int i = 0; i < inventory_->size(); ++i) {
        auto card = inventory_->at(i);
        if(card->type() == SSU::Stub)
            continue;

        ts << qSetFieldWidth(indent) << left << card->slot();
        ts << qSetFieldWidth(indent) << left << SSU::cardTypeToString(static_cast<SSU::CardType>(card->type()));
        ts << qSetFieldWidth(indent) << left << card->serialNumber();
        ts << qSetFieldWidth(indent) << left << card->software();
        ts << qSetFieldWidth(indent) << left << card->hardware();
        ts << qSetFieldWidth(1) << endl;
    }

#else
    ts << "Panel serial number " << serialNumber_ << Qt::endl;

    for(auto h : header) {
        ts << qSetFieldWidth(indent) << Qt::left << h;
    }
    ts << qSetFieldWidth(1) << Qt::endl;

    for(int i = 0; i < inventory_->size(); ++i) {
        auto card = inventory_->at(i);
        if(card->type() == SSU::Stub)
            continue;

        ts << qSetFieldWidth(indent) << Qt::left << card->slot();
        ts << qSetFieldWidth(indent) << Qt::left << SSU::cardTypeToString(static_cast<SSU::CardType>(card->type()));
        ts << qSetFieldWidth(indent) << Qt::left << card->serialNumber();
        ts << qSetFieldWidth(indent) << Qt::left << card->software();
        ts << qSetFieldWidth(indent) << Qt::left << card->hardware();
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

void ssu::Panel::clear()
{
    serialNumber_.clear();
    inventory_->clear();
}

QVariantList/*QList <ssu::Card*>*/ ssu::Panel::getCardGroup(int group, int type) const
{
    QVariantList list;
    for(int i = 0; i < panelConfiguration_->size(); ++i) {
        if(panelConfiguration_->at(i)->type() != type)
            continue;
        if(panelConfiguration_->at(i)->configuration().groupNumber() != group)
            continue;

        auto it = inventory_->constFind(panelConfiguration_->at(i)->address());
        if(it == inventory_->constEnd())
            continue;
        list.append(QVariant::fromValue(*it));
    }
    return list;
}

QList<int> ssu::Panel::cardTypes() const
{
    auto list = cardFactory_->types();
    std::sort(list.begin(), list.end());
    return list;
}

ssu::CardFactory *ssu::Panel::cardFactory() const
{
    return cardFactory_;
}
