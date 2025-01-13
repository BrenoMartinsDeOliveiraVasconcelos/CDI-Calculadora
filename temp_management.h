#ifndef TEMP_MANAGEMENT_H
#define TEMP_MANAGEMENT_H

#include <QDir>

class tempInfo{
    public:
        QString tempName = "CDICALC";
        QDir tempFolder = QDir::temp();
        QString fullFolder = tempFolder.absolutePath() + QDir::separator() + tempName;
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
