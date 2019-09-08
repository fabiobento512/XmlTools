#ifndef XMLCUSTOMCODE_H
#define XMLCUSTOMCODE_H

#include "utilxmltools.h"

#include <ctime> // script execution time calculation
#include <QScriptEngine>
#include <QTextStream>
#include <QCoreApplication>

#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

static constexpr double SLOW_SCRIPT_TIME = 0.1; // if a user script takes more than 0.1 seconds to execute give a warning.
static constexpr int CUSTOM_FILES_PER_THREAD = 4;

// Uses a singleton implementation (based on here: http://www.yolinux.com/TUTORIALS/C++Singleton.html)
// which allows each thread to keep one script engine without always create/destruct them
class XmlCustomCode
{
public:
    static XmlCustomCode* getInstance();
    void executeCustomCode(const QString &jsString, const QVector<QString> &filesToProcess, const bool backupsEnabled, const bool verboseEnabled);
private:
    static XmlCustomCode* uniqueInstance;

    const int numThreads;
    QThreadPool myThreadPool;

    struct jsCustomCodeEngine{
        /* No need to delete these... They will stay alive until program quits. */
        QScriptEngine* scriptEngine;
        QScriptValue* jsFunction;
        QScriptValue* getXmlDataFunction;
        QScriptValue* setXmlDataFunction;
        QMutex *mutexForEngine;
    };

    QVector<jsCustomCodeEngine> jsScriptEngines;

    XmlCustomCode(); // constructor is private (use getInstance)
    XmlCustomCode(XmlCustomCode const&);             // copy constructor is private
    XmlCustomCode& operator=(XmlCustomCode const&);  // assignment operator is private

    void displayJsException(QScriptEngine &engine, QScriptValue &engineResult);

    __attribute__((always_inline)) inline void customCodeUnwinding(const QString &fileName, QString &currXmlFileString,
                                                                   QScriptEngine &engine, clock_t &begin, double elapsed_secs, QScriptValue &engineResult, QScriptValue &jsFunction,
                                                                   QScriptValue &getXmlDataFunction, QScriptValue &setXmlDataFunction, const bool &backupsEnabled, const bool &verboseEnabled){
        if(backupsEnabled){
            UtilXmlTools::backupFile(fileName, verboseEnabled);
        }

        QFile currXmlFile(fileName);

        if(!currXmlFile.open(QFile::ReadOnly | QFile::Text)){
            UtilXmlTools::displayErrorMessage("@CUSTOM_CODE","Error loading '" + fileName + "' file for read operation.");
        }

        currXmlFileString=QTextStream(&currXmlFile).readAll();

        currXmlFile.close(); // close reading

        setXmlDataFunction.call(setXmlDataFunction,QScriptValueList() << currXmlFileString);

        begin = clock();

        engineResult=jsFunction.call(); // main funtion allows to use return to exit prematurely from user code

        if(verboseEnabled){
            elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC;

            // Warn the user if the script took much time
            if(elapsed_secs>SLOW_SCRIPT_TIME){
                std::cout << "Warning: Slow javascript code detected.\n" <<
                             "Warning: Script execution seconds: " << elapsed_secs
                          << std::endl;
            }
        }

        if (engine.hasUncaughtException()) {
            displayJsException(engine,engineResult);
        }

        if(!currXmlFile.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate)){
            UtilXmlTools::displayErrorMessage("@CUSTOM_CODE","Error loading '" + fileName + "' file for @CUSTOM_CODE write operation.");
        }

        engineResult=getXmlDataFunction.call();

        if (engine.hasUncaughtException()) {
            displayJsException(engine,engineResult);
        }

        QTextStream(&currXmlFile) << engineResult.toString(); // retreive the modified xml by javascript and save it to the file

        currXmlFile.close();
    }
};

#endif // XMLCUSTOMCODE_H
