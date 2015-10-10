#include "xmlcustomcode.h"

XmlCustomCode* XmlCustomCode::uniqueInstance = NULL;

QScriptValue echo(QScriptContext *context, QScriptEngine*)
{
    std::cout << context->argument(0).toString().toUtf8().constData() << std::endl;

    return "";
}

XmlCustomCode::XmlCustomCode(): numThreads(omp_get_num_procs()*2)
{
    // create individual thread script engines
    this->scriptEngines.reserve(this->numThreads);
    this->jsFunctions.reserve(this->numThreads);
    this->getXmlDataFunctions.reserve(this->numThreads);
    this->setXmlDataFunctions.reserve(this->numThreads);

    QString jsxmlString;
    QFile jsxmlfile(":/resources/libs/jsxml.js");

    jsxmlfile.open(QFile::ReadOnly | QFile::Text);

    jsxmlString=QTextStream(&jsxmlfile).readAll();

    for(int i=0; i<this->numThreads; i++){
        this->scriptEngines.append(new QScriptEngine());
        this->jsFunctions.append(new QScriptValue());

        // main needs to be called so the user code is evaluated
        // alternatively you can do: myFunc=engine.evaluate('(function main(){})'); myFunc.call();
        // Note the () around the function
        this->getXmlDataFunctions.append(new QScriptValue(this->scriptEngines.at(i)->evaluate("(function getXmlData() { return $xmlData; })")));
        this->setXmlDataFunctions.append(new QScriptValue(this->scriptEngines.at(i)->evaluate("(function setXmlData(newXmlData) { $xmlData=newXmlData; })")));

        // Add echo function so user can debug the code
        QScriptValue echoFunction = this->scriptEngines.at(i)->newFunction(echo);
        this->scriptEngines.at(i)->globalObject().setProperty("echo", echoFunction);

        // Add the js library for XmlEditing
        this->scriptEngines.at(i)->evaluate(jsxmlString);
    }
}

XmlCustomCode* XmlCustomCode::getInstance(){

    if (uniqueInstance==NULL){   // allow only one instance
        uniqueInstance = new XmlCustomCode();
    }

    return uniqueInstance;
}

void XmlCustomCode::executeCustomCode(const QString &jsString, const QVector<QString> &filesToProcess, const bool backupsEnabled, const bool verboseEnabled){

    // Reconstruct script functions
    for(int i=0; i<this->numThreads; i++){
        *this->jsFunctions[i]=this->scriptEngines.at(i)->evaluate("(function main() {"+jsString+"})");
    }

    QString currXmlFileString;

    QScriptValue engineResult; // variable to check for js_errors
    double elapsed_secs=0; // elapsed seconds that a user script took
    clock_t begin; // seconds that a script started

    // Single tread if small number of files
    if(filesToProcess.size()<CUSTOM_FILES_PER_THREAD){
        // Process all XmlFiles
        for(int i=0; i<filesToProcess.size(); i++){

            customCodeUnwinding(filesToProcess.at(i),currXmlFileString,*this->scriptEngines.at(0),begin,elapsed_secs,engineResult,
                                *this->jsFunctions.at(0),*this->getXmlDataFunctions.at(0),*this->setXmlDataFunctions.at(0),backupsEnabled,verboseEnabled);
        }
    }
    else{ // Multithread if there are many files
        // Process all XmlFiles
#pragma omp parallel for num_threads(this->numThreads) schedule(dynamic)
        for(int i=0; i<filesToProcess.size()-CUSTOM_FILES_PER_THREAD; i+=CUSTOM_FILES_PER_THREAD){

            const int tid=omp_get_thread_num();

            QString currXmlFileStringThread;

            QScriptValue engineResultThread; // variable to check for js_errors
            double elapsedSecsThread=0; // elapsed seconds that a user script took
            clock_t beginThread; // seconds that a script started

            customCodeUnwinding(filesToProcess.at(i),currXmlFileStringThread,*this->scriptEngines.at(tid),beginThread,elapsedSecsThread,engineResultThread,
                                *this->jsFunctions.at(tid),*this->getXmlDataFunctions.at(tid),*this->setXmlDataFunctions.at(tid),backupsEnabled,verboseEnabled);

            customCodeUnwinding(filesToProcess.at(i+1),currXmlFileStringThread,*this->scriptEngines.at(tid),beginThread,elapsedSecsThread,engineResultThread,
                                *this->jsFunctions.at(tid),*this->getXmlDataFunctions.at(tid),*this->setXmlDataFunctions.at(tid),backupsEnabled,verboseEnabled);
        }

        if(filesToProcess.size()%CUSTOM_FILES_PER_THREAD!=0){

            int alreadyProcessedFiles=(filesToProcess.size()/CUSTOM_FILES_PER_THREAD)*CUSTOM_FILES_PER_THREAD;

            for(int i=alreadyProcessedFiles; i<filesToProcess.size(); i++){

                customCodeUnwinding(filesToProcess.at(i),currXmlFileString,*this->scriptEngines.at(0),begin,elapsed_secs,engineResult,
                                    *this->jsFunctions.at(0),*this->getXmlDataFunctions.at(0),*this->setXmlDataFunctions.at(0),backupsEnabled,verboseEnabled);
            }
        }
    }
}

void XmlCustomCode::displayJsException(QScriptEngine &engine, QScriptValue &engineResult){
    if (engine.hasUncaughtException()) {
        UtilXmlTools::displayErrorMessage("@CUSTOM_CODE","Uncaught js exception (user code) at line " +QString::number(engine.uncaughtExceptionLineNumber()) + ":\n" + engineResult.toString());
    }
}
