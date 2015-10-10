#include "util.h"

namespace GlobalVars{
QString AppName="XmlTools";
#ifdef Q_OS_WIN
QString AppExecutable=AppName+".exe";
#else
QString AppExecutable="./"+AppName; // Mac OS needs unix like executing
#endif
QString AppVersion="2.0b";
}

namespace Util{

QString normalizePath(QString path){
    return path.replace("\\","/");
}

QString cutName(QString path){
    return path.remove(0,path.lastIndexOf('/')).remove('"');
}

QString insertQuotes(QString path){
    return "\""+path+"\"";
}

QString normalizeAndQuote(QString path){
    return insertQuotes(normalizePath(path));
}

bool checkEmptySpaces(QStringList toCheck){
    foreach (QString current, toCheck){
        if(current.trimmed().isEmpty()){
            return true; //There are empty spaces
        }
    }
    return false;
}

bool checkIfIntegers(QStringList toCheck){
    foreach (QString current, toCheck){
        if(!isStringInteger(current)){
            return true; // Some aren't valid integers
        }
    }
    return false;
}

bool checkIfDoubles(QStringList toCheck){
    foreach (QString current, toCheck){
        if(!isStringDouble(current)){
            return true; // Some aren't valid doubles
        }
    }
    return false;
}

bool isStringInteger(QString myString){
    bool isNumber;

    myString.toInt(&isNumber); //convert to int and see if it succeeds

    return isNumber;
}

bool isStringDouble(QString myString){
    bool isDouble;

    myString.toDouble(&isDouble); //convert to double and see if it succeeds

    return isDouble;
}

QString fullTrim(QString str) {

    str = str.simplified(); //convert all invisible chars in normal whitespaces
    str.replace( " ", "" );

    return str;
}

//Searches for the QString "toSearch" in the "myString" variable backward
//Returns the index of the first match or -1 if not found
int indexOfBackward(const QString &myString, const QString &toSearch, int from){
    int myStringSize=myString.size();
    int toSearchSize=toSearch.size();

    if(from==-1){
        from=myStringSize;
    }

    int i=from;

    while(i>=0){
        for(int j=toSearchSize-1; j>=0; j--){
            i--;
            if(myString.at(i)!=toSearch.at(j)){
                break;
            }
            if(j==0){
                return i;
            }
        }
    }

    return -1;
}

// Use qstring split
//QStringList substring(const QString &myString,const QString &separator, Qt::CaseSensitivity cs){
//    QStringList result = QStringList();
//    int currIdx=0, nextIdx=0;

//    while(true){
//        nextIdx=myString.indexOf(separator,currIdx,cs);
//        result << myString.mid(currIdx,nextIdx-currIdx);
//        if(nextIdx==-1) break;
//        currIdx=nextIdx+1;
//    }

//    return result;
//}

QStringList qStringListFromSpacedString(const QString &mySpacedString){
    return mySpacedString.split(" ");
}

QList<int> qListIntFromSpacedString(const QString &mySpacedString){
    QStringList stringList;
    QList<int> intList;

    stringList = mySpacedString.split(" ");

    foreach(QString value, stringList){
        intList << value.toInt();
    }

    return intList;
}

QList<double> qListDoubleFromSpacedString(const QString &mySpacedString){
    QStringList stringList;
    QList<double> doubleList;

    stringList = mySpacedString.split(" ");

    foreach(QString value, stringList){
        doubleList << value.toDouble();
    }

    return doubleList;
}

QString normalizeDecimalSeparator(QString value){
    return value.replace(',','.');
}

bool backupFile(QString file){
    return copyFile(file,file+".bak");
}

bool copyFile(QString src, QString dest){
    return QFile::copy(src, dest);
}


// Supports wildcards, and directory with wildcard e.g.:
// *.xml
// C:/myXmls/*.xml
QStringList getAllFilesByWildcard(const QString &wildcard){

    QString pathNormalized;
    QStringList nameWildCard; // entryList requires a QStringList
    QStringList resultFiles; // result files with absolute path
    int endOfPathIdx;
    QDir directory;

    if(wildcard==""){
        std::cout << "You need to specify a wildcard! Aborting..." << std::endl;
        exit(1);
    }

    pathNormalized=Util::normalizePath(wildcard); // Convert slashes to work in both mac and windows

    if(pathNormalized.contains("/")){ // If contains full path
        endOfPathIdx=Util::indexOfBackward(pathNormalized,"/"); // get last slash

        nameWildCard.append(pathNormalized.right(pathNormalized.size()-1-endOfPathIdx)); // get the names wildcard // -1 because starts with zeo

        pathNormalized=pathNormalized.left(endOfPathIdx); // get the complete path

        directory=QDir(pathNormalized);
    }
    else{ // if relative
        nameWildCard << wildcard;
    }

    foreach (QFileInfo fileInfo, directory.entryInfoList(nameWildCard)){
        resultFiles << fileInfo.absoluteFilePath();
    }

    return resultFiles;
}

}
