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

#ifndef QCSV_H
#define QCSV_H

#include <vector>
#include <QString>
#include <QFile>

using namespace std;
// O delimitador é SEMPRE vírgula. Use bibliotecas propias para csv para um sistema mais completo.

char separator = ';';

QString generateCSVLine(vector<QString> headers){
    QString header = "";
    for (size_t i=0; i<headers.size(); i++){
        header.append(headers[i]+separator);
    };

    header.removeLast();
    header.append("\n");

    return header;
};

vector<int> getColumnAndRowCount(QString csvPath){
    QFile csvF(csvPath);
    vector<int> count = {0, 0};

    if (csvF.open(QIODevice::ReadOnly)){
        int rows = 0;
        int columns = 0;

        while (!csvF.atEnd()){
            QString line = csvF.readLine();
            QStringList lineContent = line.split(separator);

            columns = lineContent.length();
            rows++;
        }

        count = {columns, rows};
    };

    return count;
};

#endif // QCSV_H
