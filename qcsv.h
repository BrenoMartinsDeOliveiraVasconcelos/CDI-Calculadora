#ifndef QCSV_H
#define QCSV_H

#include <vector>
#include <QString>

using namespace std;
// O delimitador é SEMPRE vírgula. Use bibliotecas propias para csv para um sistema mais completo.

QString generateCSVLine(vector<QString> headers){
    QString header = "";
    for (size_t i=0; i<headers.size(); i++){
        header.append(headers[i]+",");
    };

    header.removeLast();
    header.append("\n");

    return header;
};

#endif // QCSV_H
