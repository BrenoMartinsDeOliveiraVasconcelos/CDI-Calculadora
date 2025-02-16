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

#ifndef CALCFUNCTIONS_H
#define CALCFUNCTIONS_H

#include <vector>
#include <QtCore>

using namespace std;

vector<vector<double>> calcYearIndex(double taxaMes, int diasAno, int anoAtual){

    // A taxa diaria por mes
    vector<double> taxaDiariaPorMes = {};
    vector<double> taxaMensal = {};

    int mes = 1;
    int numDiasMes = 0;
    for (int dia = 1; dia <= diasAno; ++dia){
        QDate data = QDate::fromJulianDay(QDate(anoAtual, 1, 1).toJulianDay() + dia - 1);

        if (data.dayOfWeek() == Qt::Saturday || data.dayOfWeek() == Qt::Sunday){
            continue;
        };

        ++numDiasMes;

        if (data.month() > mes || dia==diasAno){
            double taxaDiaria = ((taxaMes*100)/numDiasMes)/100;

            taxaDiariaPorMes.push_back(taxaDiaria);
            taxaMensal.push_back(taxaDiaria*numDiasMes);

            ++mes;
            numDiasMes = 0;
        };

    };

    return {taxaDiariaPorMes, taxaMensal};
}

#endif // CALCFUNCTIONS_H
