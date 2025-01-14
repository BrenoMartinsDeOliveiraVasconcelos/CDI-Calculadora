#ifndef QTDECIMALCONVERSION_H
#define QTDECIMALCONVERSION_H

#include <QString>

int decimals = 2;

QString convertFQString(float num){
    return QString::number(num, 'f', decimals);
};

#endif // QTDECIMALCONVERSION_H
