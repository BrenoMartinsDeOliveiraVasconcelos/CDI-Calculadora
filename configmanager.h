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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iostream>
#include <QDir>
#include <QFile>
#include <map>

using namespace std;

class configuration{
public:
    // Gerar configurações
bool generateConfigFolder(bool resetConfig = false){
    // Como isso não quebrou o programa? '-'
    //configuration configs;

    if (!homeFolder.mkdir(folderName) && !homeFolder.exists(folderName)){
        return false;
    }else{
        QDir folderAbsPathObj(folderAbsPath);
        QFile file(configFile);

        if (!folderAbsPathObj.exists(filename)){
            bool isOpen = file.open(QIODevice::WriteOnly);

            if (!isOpen){
                return false;
            };
        }else{
            if (!resetConfig){
                return true;
            }else{
                bool isOpen = file.open(QIODevice::WriteOnly);

                if (!isOpen){
                    return false;
                }
            }
        };

        file.close();

        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);

            stream << "cdi=100,00\n" << "selic=15,00\n" << "path=*\n" << "autorel=0\n" << "acaorel=1\n" << "w=1280\n" << "h=768\n";

            file.close();
            return true;
        }else{
            return false;
        };
    };
};

map<QString, QString> getConfig(){
    map<QString, QString> errorMap;
    errorMap["ERROR"] = "!!!";

    QFile file(configFile);

    if (!file.exists()){
        return errorMap;
    };

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        cout << "IF STATE";
        while (!file.atEnd()) {
            QString line = file.readLine();

            // SPlit com o token '='
            QStringList lineContent = line.split('=');

            if (lineContent.length() < 2){
                continue;
            }

            configs[lineContent[0]] = lineContent[1].replace('\n', "");
        }

        return configs;
    }else{
        return errorMap;
    }

    return configs;

};

bool clearConfig(){
    QFile file(configFile);

    if (!file.open(QIODevice::WriteOnly)){
        return false;
    }else{
        QTextStream out(&file);
        out << "";
        return true;
    };
}

bool setConfig(QString config, QString val){
    QFile file(configFile);

    if (file.open(QIODevice::Append | QIODevice::Text)){
        QTextStream stream(&file);

        stream << config + "=" + val + "\n";
        return true;
    }else{
        return false;
    };
}

bool isConfigPathCreated(){
    QFile file = configFile;

    return file.exists();
};

private:
    QString folderName = ".cdicfg";
    QDir homeFolder = QDir::home();
    QString folderAbsPath = homeFolder.absolutePath()+QDir::separator()+folderName;
    QString filename = "cfg.txt";
    QString configFile = folderAbsPath+QDir::separator()+filename;
    map<QString, QString> configs;
};


#endif // CONFIGMANAGER_H
