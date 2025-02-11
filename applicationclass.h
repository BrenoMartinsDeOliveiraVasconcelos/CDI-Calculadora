#ifndef APPLICATIONCLASS_H
#define APPLICATIONCLASS_H

#include <vector>
#include <QString>
#include <configmanager.h>
#include <map>
#include <QCoreApplication>
#include <QApplication>

#if defined(_WIN64)
    #define big    long long
#else
    #define big     long
#endif

using namespace std;

class runtimeConsts{
public:
    int restartCode(){
        return RESTART_CODE;
    };

    vector<double> iof(){
        return IOF_RATE;
    };

    unsigned big int maxMoneyValue(){
        return MAX_MONETARY_VAL;
    };

    unsigned int maxDays(){
        return MAX_DAYS;
    };

    big int maxTableNum(){
        return MAX_TABLE_VALUE;
    };

    QString tooBig(){
        return TOO_BIG_SYMB;
    };

private:
    int RESTART_CODE = 128;
    // Hard coded
    vector<double> IOF_RATE = {
        0.96, 0.93, 0.90,
        0.86, 0.83, 0.80,
        0.76, 0.73, 0.70,
        0.66, 0.63, 0.60,
        0.56, 0.53, 0.50,
        0.46, 0.43, 0.40,
        0.36, 0.33, 0.30,
        0.26, 0.23, 0.20,
        0.16, 0.13, 0.10,
        0.06, 0.03, 0.00
    };

    big unsigned int MAX_MONETARY_VAL = 9223372036854775807;
    unsigned int MAX_DAYS = 31;
    big int MAX_TABLE_VALUE = 9223372036854775807;
    QString TOO_BIG_SYMB = "---";
};


class appManager{
public:
    void delay()
    {
        configuration config;

        map<QString, QString> configs = config.getConfig();
        double delayT = configs["delay"].toDouble();

        QTime dieTime= QTime::currentTime().addMSecs(delayT);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    void restart(){
        runtimeConsts consts;


        qApp->exit(consts.restartCode());
    }


    void exit(int code){
        qApp->exit(code);
    }
};

#endif // APPLICATIONCLASS_H
