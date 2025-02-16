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
