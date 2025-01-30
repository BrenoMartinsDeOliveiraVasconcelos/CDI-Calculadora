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
#include <QDir>

using namespace std;

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
    vector<vector<QString>> content;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return content;
    }

    QTextStream in(&file);
    bool isHeaderSkipped = !ignoreHeader;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (!isHeaderSkipped) {
            isHeaderSkipped = true;
            continue;
        }

        QStringList fields = line.split(QString(separator), Qt::KeepEmptyParts);

        vector<QString> row;
        for (const QString& field : fields) {
            row.push_back(field);
        }

        content.push_back(row);
    }

    file.close();
    return content;
};


bool writeCSV(vector<QString> headers, vector<vector<QString>> lines, QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    // Write headers
    QStringList headerList;
    for (const QString& h : headers) {
        headerList << h;
    }
    QString headerLine = headerList.join(QString(separator)) + "\n";
    if (file.write(headerLine.toUtf8()) == -1) {
        file.close();
        return false;
    }

    // Write each line
    for (const vector<QString>& line : lines) {
        QStringList lineList;
        for (const QString& field : line) {
            lineList << field;
        }
        QString lineStr = lineList.join(QString(separator)) + "\n";
        if (file.write(lineStr.toUtf8()) == -1) {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

#endif // QCSV_H
