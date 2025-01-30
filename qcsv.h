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

#include <iostream>

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

vector<int> getColumnAndRowCount(QString csvPath, bool ignoreHeader=false){
    QFile csvF(csvPath);
    vector<int> count = {0, 0};
    if (csvF.open(QIODevice::ReadOnly)){
        int rows = 1;
        int columns = 1;
        bool headerSkipped = false;

        while (!csvF.atEnd()){
            QString line = csvF.readLine().trimmed();
            if (line.isEmpty()) continue;  // Skip empty lines

            if (ignoreHeader && !headerSkipped){
                headerSkipped = true;
                continue;
            }

            QStringList lineContent = line.split(separator);
            columns = lineContent.length();
            rows++;

            //cout << rows << endl;
        }
        count = {rows, columns};
    }
    return count;
}

QStringList getHeaders(QString path){
    QStringList headers = {""};

    QFile csvF(path);
    if (csvF.open(QIODevice::ReadOnly)){
        int lineNum = 0;

        while (!csvF.atEnd()){
            lineNum++;

            QString line = csvF.readLine();

            if (lineNum == 1){
                headers = line.split(separator);

                // Limpar caractéres \n
                int index = 0;
                for (auto h:headers){
                    headers[index] = h.replace("\n", "");

                    index++;
                };
            };
        };
    };

    return headers;
}

vector<vector<QString>> getCSVContent(QString path, bool ignoreHeader = false) {
    vector<vector<QString>> content; // Inicializa o vetor de vetores vazio

    QFile csvF(path);

    if (csvF.open(QIODevice::ReadOnly)) {
        int row = 0;

        while (!csvF.atEnd()) {
            QString line = csvF.readLine().trimmed(); // Remove quebras de linha e espaços no início/fim

            if (ignoreHeader && row == 0) {
                row++;
                continue;
            }

            QStringList lineContent = line.split(separator); // Use o separador adequado (ajuste se necessário)
            vector<QString> rowContent;

            // Navegar pela linha e adicionar ao vetor de QString
            for (const auto &c : lineContent) {
                rowContent.push_back(c.trimmed()); // Remove espaços desnecessários
            }

            // Adicionar a linha completa no conteúdo
            content.push_back(rowContent);

            row++;
        }
    }

    return content;
};

#endif // QCSV_H
