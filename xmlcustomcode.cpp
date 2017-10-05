#include "xmlcustomcode.h"

XmlCustomCode* XmlCustomCode::uniqueInstance = NULL;

QScriptValue echo(QScriptContext *context, QScriptEngine*)
{
    std::cout << context->argument(0).toString().toUtf8().constData() << std::endl;

    return "";
}

XmlCustomCode::XmlCustomCode(): numThreads(QThread::idealThreadCount())
{
    myThreadPool.setMaxThreadCount(numThreads);
    myThreadPool.setExpiryTimeout(-1); // don't let threads expire

    // create individual thread script engines
    this->jsScriptEngines.reserve(this->numThreads);

    QString jsxmlString;
    QFile jsxmlfile(":/resources/libs/jsxml.js");

    jsxmlfile.open(QFile::ReadOnly | QFile::Text);

    jsxmlString=QTextStream(&jsxmlfile).readAll();

    for(int i=0; i<this->numThreads; i++){

        jsCustomCodeEngine e;

        e.scriptEngine = new QScriptEngine();
        e.jsFunction = new QScriptValue();

        // main needs to be called so the user code is evaluated
        // alternatively you can do: myFunc=engine.evaluate('(function main(){})'); myFunc.call();
        // Note the () around the function
        e.getXmlDataFunction = new QScriptValue(e.scriptEngine->evaluate("(function getXmlData() { return $xmlData; })"));
        e.setXmlDataFunction = new QScriptValue(e.scriptEngine->evaluate("(function setXmlData(newXmlData) { $xmlData=newXmlData; })"));
        e.isAvailable = true;

        // Add echo function so user can debug the code
        QScriptValue echoFunction = e.scriptEngine->newFunction(echo);
        e.scriptEngine->globalObject().setProperty("echo", echoFunction);

        // Add the js library for XmlEditing
        e.scriptEngine->evaluate(jsxmlString);

        this->jsScriptEngines.append(e);
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
        *(jsScriptEngines[i].jsFunction) = jsScriptEngines.at(i).scriptEngine->evaluate("(function main() {"+jsString+"})");
    }

    QString currXmlFileString;

    QScriptValue engineResult; // variable to check for js_errors
    double elapsed_secs=0; // elapsed seconds that a user script took
    clock_t begin; // seconds that a script started

    // Single tread if small number of files or number of threads = 1
    if(filesToProcess.size()<=CUSTOM_FILES_PER_THREAD || numThreads == 1){

        jsCustomCodeEngine &jsEngine = getAvailableJsEngine();

        // Process all XmlFiles
        for(int i=0; i<filesToProcess.size(); i++){
            customCodeUnwinding(filesToProcess.at(i),currXmlFileString,*jsEngine.scriptEngine,begin,elapsed_secs,engineResult,
                                *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);
        }
    }
    else{ // Multithread if there are many files
        // Process all XmlFiles

        QVector<QFuture<void>> executingThreads;

        for(int i=0; i<=filesToProcess.size()-CUSTOM_FILES_PER_THREAD; i+=CUSTOM_FILES_PER_THREAD){

            executingThreads <<
            QtConcurrent::run(&this->myThreadPool, [=]()
            {
                mutexIsAvailable.lock();
                jsCustomCodeEngine &jsEngine = getAvailableJsEngine();
                jsEngine.isAvailable = false;
                mutexIsAvailable.unlock();

                QString currXmlFileStringThread;

                QScriptValue engineResultThread; // variable to check for js_errors
                double elapsedSecsThread=0; // elapsed seconds that a user script took
                clock_t beginThread; // seconds that a script started

                customCodeUnwinding(filesToProcess.at(i),currXmlFileStringThread,*jsEngine.scriptEngine,beginThread,elapsedSecsThread,engineResultThread,
                                    *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);

                customCodeUnwinding(filesToProcess.at(i+1),currXmlFileStringThread,*jsEngine.scriptEngine,beginThread,elapsedSecsThread,engineResultThread,
                                    *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);

                customCodeUnwinding(filesToProcess.at(i+2),currXmlFileStringThread,*jsEngine.scriptEngine,beginThread,elapsedSecsThread,engineResultThread,
                                    *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);

                customCodeUnwinding(filesToProcess.at(i+3),currXmlFileStringThread,*jsEngine.scriptEngine,beginThread,elapsedSecsThread,engineResultThread,
                                    *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);

                mutexIsAvailable.lock();
                jsEngine.isAvailable = true;
                mutexIsAvailable.unlock();
            });
        }

        // Wait for all threads to finish
        for(QFuture<void> &f :executingThreads){
            f.waitForFinished();
        }

        if(filesToProcess.size()%CUSTOM_FILES_PER_THREAD!=0){

            int alreadyProcessedFiles=(filesToProcess.size()/CUSTOM_FILES_PER_THREAD)*CUSTOM_FILES_PER_THREAD;

            jsCustomCodeEngine &jsEngine = getAvailableJsEngine();

            for(int i=alreadyProcessedFiles; i<filesToProcess.size(); i++){

                customCodeUnwinding(filesToProcess.at(i),currXmlFileString,*jsEngine.scriptEngine,begin,elapsed_secs,engineResult,
                                    *jsEngine.jsFunction,*jsEngine.getXmlDataFunction,*jsEngine.setXmlDataFunction,backupsEnabled,verboseEnabled);
            }
        }
    }
}

void XmlCustomCode::displayJsException(QScriptEngine &engine, QScriptValue &engineResult){
    if (engine.hasUncaughtException()) {
        UtilXmlTools::displayErrorMessage("@CUSTOM_CODE","Uncaught js exception (user code) at line " +QString::number(engine.uncaughtExceptionLineNumber()) + ":\n" + engineResult.toString());
    }
}

XmlCustomCode::jsCustomCodeEngine& XmlCustomCode::getAvailableJsEngine(){
    for(jsCustomCodeEngine &e : this->jsScriptEngines){
        if(e.isAvailable){
            return e;
        }
    }

    throw std::runtime_error("Could't find an available js engine for custom command.");
}
