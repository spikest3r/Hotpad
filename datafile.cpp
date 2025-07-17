#include "datafile.h"
#include "globals.h"
#include "actions.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

QString getFileName() { // ensure folder exists
    QString folder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Hotpad";
    QDir dir(folder);
    if(!dir.exists()) {
        dir.mkpath(".");
    }
    return folder + "/settings"; // file name without extension
}

QStringList split(const QString &line){
    QString cleanedLine = line;
    if (cleanedLine.endsWith("|EOL|\n"))
        cleanedLine.chop(6);

    return cleanedLine.split('\x1F');
}

void SaveSettings() {
    QFile file(getFileName());
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for(int i = 0; i < 10; i++) { // only 10 actiondata. 1 for each numpad key
            ActionData data = hotkeyActions[i];
            QString comboKeys = "";
            for(int j = 0; j < 3; j++) {
                comboKeys+= data.comboKeys[j] ? "1" : "0";
                comboKeys+= ",";
            }
            out << QString::number((int)data.type) << "\x1F"
                << data.path << "\x1F"
                << comboKeys << "\x1F"
                << data.letter << "|EOL|\n";
        }
        file.close();
    }
}

void LoadSettings() {
    QFile file(getFileName());
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int i = 0;
        while(!in.atEnd()) {
            QString line = in.readLine();
            if(line.trimmed().isEmpty()) continue;
            QStringList fields = split(line);
            if(fields.size() < 4) continue;

            ActionType type = (ActionType)fields[0].toInt();
            QString path = fields[1];
            QString comboKeysStr = fields[2];
            QStringList list = comboKeysStr.split(',');
            QString letter = fields[3];
            for(int j = 0; j < 3; j++) {
                hotkeyActions[i].comboKeys[j] = list[j] == "1";
            }
            hotkeyActions[i].path = path.toStdWString();
            hotkeyActions[i].type = type;
            hotkeyActions[i].letter = letter.split("|")[0]; // split EOL and letter
            i++;
        }
    }
}
