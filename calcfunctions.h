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
