#ifndef MISCQ_H
#define MISCQ_H

#include <QString>
#include <configmanager.h>
#include <map>
#include <QCoreApplication>

using namespace std;

void delay()
{
    configuration config;

    map<QString, QString> configs = config.getConfig();
    double delayT = configs["delay"].toDouble();

    QTime dieTime= QTime::currentTime().addMSecs(delayT);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


#endif // MISCQ_H
