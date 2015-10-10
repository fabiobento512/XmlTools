#ifndef UTIL_H
#define UTIL_H

#include <QFile>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QVector>
#include <iostream> // cout, cin etc.

namespace GlobalVars{
extern QString AppName;
extern QString AppExecutable;
extern QString AppVersion;
}

/**
  Utilities functions (global)
  **/
namespace Util{
QString normalizePath(QString path);
QString cutName(QString path);
QString insertQuotes(QString path);
QString normalizeAndQuote(QString path);
QString fullTrim(QString str);
QString normalizeDecimalSeparator(QString value);
// Use qstring split
// QStringList substring(const QString &myString, const QString &separator, Qt::CaseSensitivity cs = Qt::CaseSensitive);
QStringList qStringListFromSpacedString(const QString &mySpacedString);
QStringList getAllFilesByWildcard(const QString &wildcard);
QList<int> qListIntFromSpacedString(const QString &mySpacedString);
QList<double> qListDoubleFromSpacedString(const QString &mySpacedString);
int indexOfBackward(const QString &myString, const QString &toSearch, int from = -1);
bool checkEmptySpaces(QStringList toCheck);
bool checkIfIntegers(QStringList toCheck);
bool checkIfDoubles(QStringList toCheck);
bool isStringInteger(QString myString);
bool isStringDouble(QString myString);
bool backupFile(QString file);
bool copyFile(QString src, QString dest);
// The commented code bellow is a big mistake because toLatin1() creates a temp object that gets
// destroyed after the semicolon: https://qt-project.org/forums/viewthread/12885 (Volker answer)
//// Convert a QString to a c string
//// Caution don't use as for example: std::cout << toCstr(a) << " " << toCstr(b);
//// as the result will be always a.
//inline const char* toCstr(const QString &myString){
//    return myString.toUtf8().constData();
//}
// Converts a std::string to QString
inline QString toQString(std::string myString){
    return QString::fromStdString(myString);
}

//TODO Needs optimization
inline QStringList QStringToArgsArray(const QString &args){
    QStringList result;
    int startIdx=0, endIdx=0;

    if(!args.isEmpty()){ // if non empty arguments

        while(endIdx<args.length()){

            startIdx=endIdx;

            if(args.at(startIdx)==' '){ // Ignore spaces until a different char is found
                endIdx++;
                continue;
            }
            else if(args.at(startIdx)!='"'){ // if first character is different from quote it ends with space
                endIdx=args.indexOf(' ',startIdx+1);
            }
            else{ // If is a quote try to get the all the string until next quote
                endIdx=args.indexOf('"',startIdx+1);
            }

            if(endIdx==-1) break;

            endIdx++;

            result << args.mid(startIdx,endIdx-startIdx).replace("\"","").trimmed(); // remove quotes
        }

    }

    return result;
}

}

#endif // UTIL_H
