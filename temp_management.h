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

#ifndef TEMP_MANAGEMENT_H
#define TEMP_MANAGEMENT_H

#include <QDir>
#include <QFile>
#include <QTextStream>

class tempInfo{
    public:
        QString tempName = "CDICALC";
        QDir tempFolder = QDir::temp();
        QString fullFolder = tempFolder.absolutePath() + QDir::separator() + tempName;
        QString tempFolderAbsolute = fullFolder+QDir::separator();
        QDir fullFolderDir = fullFolder;
};

bool createTempFolder(){
    tempInfo temp;

    return temp.tempFolder.mkdir(temp.tempName);
};

bool deleteTempFolder(){
    tempInfo temp;

    return temp.tempFolder.rmdir(temp.tempName);
};


bool tempFolderExists(){
    tempInfo temp;

    return temp.tempFolder.exists(temp.tempName);
};

#endif // TEMP_MANAGEMENT_H
