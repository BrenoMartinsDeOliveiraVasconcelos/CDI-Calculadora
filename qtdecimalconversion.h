/*   Calculadora de rendimento do CDI.
*    Copyright (C) 2025  Breno Martins
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QTDECIMALCONVERSION_H
#define QTDECIMALCONVERSION_H

#include <QString>
#include <applicationclass.h>

int decimals = 2;

QString convertFQString(long double num){
    runtimeConsts runtime;

    QString resultado;

    if (num <= runtime.maxTableNum()){
        resultado = QString::number(num, 'f', decimals);
    }else{
        resultado = runtime.tooBig();
    };

    return resultado;
};

#endif // QTDECIMALCONVERSION_H
