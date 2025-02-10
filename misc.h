#ifndef MISC_H
#define MISC_H

#include <QString>
#include <configmanager.h>
#include <map>
#include <QCoreApplication>

using namespace std;

void delay()
{
    configuration config;

    map<QString, QString> configs = config.getConfig();
    double delay = configs["delay"].toDouble();

    QTime dieTime= QTime::currentTime().addMSecs(delay);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


#endif // MISC_H
