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
bool generateConfigFolder(){
    configuration configs;

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
            return true;
        };

        file.close();

        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);

            stream << "cdi=100.00\n" << "selic=15.00\n" << "save=*\n";

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

private:
    QString folderName = ".cdicfg";
    QDir homeFolder = QDir::home();
    QString folderAbsPath = homeFolder.absolutePath()+QDir::separator()+folderName;
    QString filename = "cfg.txt";
    QString configFile = folderAbsPath+QDir::separator()+filename;
    map<QString, QString> configs;
};


#endif // CONFIGMANAGER_H
