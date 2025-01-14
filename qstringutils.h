#ifndef QSTRINGUTILS_H
#define QSTRINGUTILS_H

#include <QString>
#include <vector>
#include <QDir>

using namespace std;

QString mergeStrings(vector<QString> strings){
    QString fullString = "";

    for (auto val:strings){
        fullString.append(val);
    };

    return fullString;
};


QString pathString(vector<QString> paths){
    QString path = "";

    for (auto val:paths){
        path.append(val+QDir::separator());
    }

    return path;
};

#endif // QSTRINGUTILS_H
