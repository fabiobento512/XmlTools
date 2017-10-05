#include "xmlpatch.h"

XmlPatch::XmlPatch(QString patchFilesWildcard, QString forceTargetFilesWildcard, bool noBackups, bool noVerbose)
{
    this->patchFilesToProcess=UtilXmlTools::getAllPatchFilesByWildcard(patchFilesWildcard);
    this->forceTargetFilesWildcard=forceTargetFilesWildcard;
    this->backupsEnabled=!noBackups;
    this->verboseEnabled=!noVerbose;

    if(forceTargetFilesWildcard!=""){
        std::cout << "User forced patch in the target file(s): " << forceTargetFilesWildcard.toUtf8().constData() << std::endl;
    }

    if(this->patchFilesToProcess.isEmpty()){
        UtilXmlTools::displayErrorMessage("Loading patch files","No .patch or .oni-patch files were found for the wildcard: "+patchFilesWildcard);
    }

}

void XmlPatch::readAndProcessPatchFile(){

    // Process all PatchFiles
    for(int i=0; i<this->patchFilesToProcess.size(); i++){

        QFile inputFile(this->patchFilesToProcess[i]);

        if (inputFile.open(QIODevice::ReadOnly))
        {

            QTextStream fileStream(&inputFile);

            checkPatchVersion(this->patchFilesToProcess[i], fileStream);
            checkAndProcessValidCommands(fileStream);

            inputFile.close();
        }
        else{
            UtilXmlTools::displayErrorMessage("Read patch file", "Error opening patch file: '" + this->patchFilesToProcess[i] + "'.\n" + inputFile.errorString());
        }

    }

    UtilXmlTools::displaySuccessMessage(this->patchFilesToProcess.size(),"Patch File(s)");

}

void XmlPatch::insertNodesOperation(const QString &xmlString, XmlFilter &filters, const QString &xPathExpression, const QString &filesWildcard){

    QVector<QString> filesToProcess;
    QList<pugi::xml_node> nodesToInsertion;
    pugi::xml_document newNode;
    pugi::xml_parse_result result;

    try{

        filesToProcess=UtilXmlTools::getAllXmlFilesByWildcard(filesWildcard);

        if(filesToProcess.isEmpty()){
            UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES","No XML files were found for the wildcard: "+filesWildcard);
        }

        result=newNode.load(xmlString.toUtf8().constData()); // load xml to insert

        if(result.status!=pugi::status_ok){
            UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES", "The xml node to insert is invalid.\n" + QString(result.description()));
        }

        // Process all XmlFiles
        for(int i=0; i<filesToProcess.size(); i++){

            UtilXmlTools::loadXmlFile(filesToProcess[i],this->document,this->rootNode,this->backupsEnabled,this->verboseEnabled,"@ADD_INSIDE_NODES");

            // Check how the element will be fetched via element name or xpath expression
            if(xPathExpression.isEmpty()){
                UtilXmlTools::getAllNamedElements(this->rootNode,nodesToInsertion,filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(xPathExpression,this->document,nodesToInsertion);
            }

            if(nodesToInsertion.isEmpty() || nodesToInsertion[0].type()==pugi::node_null){

                QString errMessage;

                if(xPathExpression.isEmpty()){
                    errMessage = "No node was found with an ElementName: '" + filters.getElementName() + "'";
                    if(filters.getParentElementName()!=""){
                        errMessage += " and a ParentElementName: '" + filters.getParentElementName() + "'";
                    }
                    if(filters.getAttributeName()!=""){
                        errMessage += " and an AttributeName: '" + filters.getAttributeName() + "' and an AttributeValue: '" + filters.getAttributeValue() + "'";
                    }
                }
                else{
                    errMessage = "No node was found with an XPathExpression: '" + xPathExpression + "'";
                }

                UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES",errMessage);
            }

            for(int j=0; j<nodesToInsertion.size(); j++){
                for (pugi::xml_node currNodeToInsert = newNode.first_child(); currNodeToInsert; currNodeToInsert = currNodeToInsert.next_sibling())
                {
                    nodesToInsertion[j].append_copy(currNodeToInsert); // append the new node
                }

            }

            UtilXmlTools::saveXmlFile(filesToProcess[i],this->document, "@ADD_INSIDE_NODES");

            nodesToInsertion.clear();
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(filesToProcess.size(),"@ADD_INSIDE_NODES");
}

void XmlPatch::removeNodesOperation(XmlFilter &filters, const QString &xPathExpression, const QString &filesWildcard){

    QVector<QString> filesToProcess;

    QList<pugi::xml_node> nodesToDeletion;

    try{

        filesToProcess=UtilXmlTools::getAllXmlFilesByWildcard(filesWildcard);

        if(filesToProcess.isEmpty()){
            UtilXmlTools::displayErrorMessage("@REMOVE_NODES","No XML files were found for the wildcard: "+filesWildcard);
        }

        // Process all XmlFiles
        for(int i=0; i<filesToProcess.size(); i++){

            UtilXmlTools::loadXmlFile(filesToProcess[i],this->document,this->rootNode,this->backupsEnabled,this->verboseEnabled,"@REMOVE_NODES");

            // Check how the element will be fetched via element name or xpath expression
            if(xPathExpression.isEmpty()){
                UtilXmlTools::getAllNamedElements(this->rootNode,nodesToDeletion,filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(xPathExpression,this->document,nodesToDeletion);
            }

            if(nodesToDeletion.isEmpty() || nodesToDeletion[0].type()==pugi::node_null){

                QString errMessage;

                if(xPathExpression.isEmpty()){
                    errMessage = "No node was found with an ElementName: '" + filters.getElementName() + "'";
                    if(filters.getParentElementName()!=""){
                        errMessage += " and a ParentElementName: '" + filters.getParentElementName() + "'";
                    }
                    if(filters.getAttributeName()!=""){
                        errMessage += " and an AttributeName: '" + filters.getAttributeName() + "' and an AttributeValue: '" + filters.getAttributeValue() + "'";
                    }
                }
                else{
                    errMessage = "No node was found with an XPathExpression: '" + xPathExpression + "'";
                }

                UtilXmlTools::displayErrorMessage("@REMOVE_NODES",errMessage);
            }

            // Delete all the specified nodes
            for(int j=0; j<nodesToDeletion.size(); j++){
                if(!nodesToDeletion[j].parent().remove_child(nodesToDeletion[j])){  // remove the node

                    QString errMessage;
                    if(xPathExpression.isEmpty()){
                        errMessage = "Couldn't remove the node with Element '" + filters.getElementName() + "'";

                        if(filters.getParentElementName()!=""){
                            errMessage += " and a ParentElement: '" + filters.getParentElementName() + "'";
                        }
                    }
                    else{
                        errMessage = "Couldn't remove the node with the XPathExpression '" + xPathExpression + "'";
                    }

                    UtilXmlTools::displayErrorMessage("@REMOVE_NODES",errMessage);
                }
            }

            UtilXmlTools::saveXmlFile(filesToProcess[i],this->document, "@REMOVE_NODES");

            nodesToDeletion.clear();
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("@REMOVE_NODES", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(filesToProcess.size(), "@REMOVE_NODES");
}

void XmlPatch::executeCommandOperation(const QString &commandString){

    // Avoid infinite fork loops
    if(commandString.contains("-p ") || commandString.contains("--patch-files ")){
        UtilXmlTools::displayErrorMessage("@COMMAND","Use of --patch-files option is not allowed inside a patch file");
    }

    // Reserved to AEI
    if(commandString.contains("--aei-patch-files-list ")){
        UtilXmlTools::displayErrorMessage("@COMMAND","Use of --aei-patch-files-list option is not allowed inside a patch file");
    }

    std::cout << "@COMMAND patch operation output:\n"
              << "########################################################################"
              << std::endl;

    OptionsParser myParser(UtilXmlTools::QStringToArgsArray(commandString));
    myParser.parse();

    std::cout
            << "########################################################################"
            << std::endl;

    UtilXmlTools::displaySuccessMessage(1,"@COMMAND");
}

void XmlPatch::executeCustomCommandOperation(const QString &jsString, const QString &filesWildcard){

    QVector<QString> filesToProcess=UtilXmlTools::getAllXmlFilesByWildcard(filesWildcard);

    try{
        if(filesToProcess.isEmpty()){
            UtilXmlTools::displayErrorMessage("@CUSTOM_CODE","No XML files were found for the wildcard: "+filesWildcard);
        }

        XmlCustomCode::getInstance()->executeCustomCode(jsString,filesToProcess,this->backupsEnabled,this->verboseEnabled);
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("@CUSTOM_CODE", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(filesToProcess.size(), "@CUSTOM_CODE");
}

void XmlPatch::checkPatchVersion(const QString &file, QTextStream &fileStream){

    QString line, patchVersion="";

    // First get the patch version and check its validity
    while ( !fileStream.atEnd() ){
        line = fileStream.readLine();

        if(line.startsWith('#')){ // Ignore comments
            continue;
        }
        else if(line.startsWith("@XML_TOOLS")){

            patchVersion=getPatchParameterValue(line,"Version");

            if(!patchVersion.startsWith("2.0")){
                QString errMessage;

                errMessage = "The current patch version is incompatible with this XmlTools version:\n";
                errMessage += "Patch file name: '" + file + "'\n";
                errMessage += "XmlTools version:  " + GlobalVars::AppVersion + "\n" + "CurrPatch version: " + patchVersion + "";
                UtilXmlTools::displayErrorMessage("@XML_TOOLS",errMessage);
            }
            break; // We have got what we wanted
        }
    }

    if(patchVersion==""){
        UtilXmlTools::displayErrorMessage("@XML_TOOLS","Patch version not found.");
    }

}

void XmlPatch::checkAndProcessValidCommands(QTextStream &fileStream){

    QString line, filesWildcard;
    QString xmlToInsert, command, jsCode;
    QString xPathExpression;
    XmlFilter filters;

    // Process the rest of the commands in patch file
    while ( !fileStream.atEnd() ){
        line = fileStream.readLine();

        if(line.startsWith('#')){ // Ignore comments
            continue;
        }
        else if(line.startsWith("@ADD_INSIDE_NODES")){
            xPathExpression=getPatchParameterValue(line,"XPathExpression");
            filters.setElementName(getPatchParameterValue(line,"ElementName"));
            filters.setParentElementName(getPatchParameterValue(line,"ParentElementName"));
            filters.setAttributeName(getPatchParameterValue(line,"AttributeName"));
            filters.setAttributeValue(getPatchParameterValue(line,"AttributeValue"));

            if(this->forceTargetFilesWildcard!=""){
                filesWildcard=this->forceTargetFilesWildcard;
            }
            else{
                filesWildcard=getPatchParameterValue(line,"Files");
            }

            // Check options
            if(xPathExpression.isEmpty() && filters.getElementName().isEmpty()){
                UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES","ElementName option or XPathExpression option is required.");
            }
            else if(!xPathExpression.isEmpty() && !filters.getElementName().isEmpty()){
                UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES","ElementName option and XPathExpression option cannot be used simultaneously.");
            }
            if(filters.getAttributeName()!="" && filters.getAttributeValue()==""){
                UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES","AttributeValue option is required if using AttributeName option.");
            }

            if(filters.getAttributeValue()!="" && filters.getAttributeName()==""){
                UtilXmlTools::displayErrorMessage("@ADD_INSIDE_NODES","AttributeName option is required if using AttributeValue option.");
            }

            while ( !fileStream.atEnd() && !line.startsWith("</xml>")){
                line = fileStream.readLine();

                if(!line.startsWith("<xml>") && !line.startsWith("</xml>")){
                    xmlToInsert += line + "\n";
                }
            }

            insertNodesOperation(xmlToInsert,filters,xPathExpression,filesWildcard);

            xmlToInsert.clear();
            filters.clear();
            xPathExpression.clear();
            filesWildcard.clear();
        }
        else if(line.startsWith("@REMOVE_NODES")){

            xPathExpression=getPatchParameterValue(line,"XPathExpression");
            filters.setElementName(getPatchParameterValue(line,"ElementName"));
            filters.setParentElementName(getPatchParameterValue(line,"ParentElementName"));
            filters.setAttributeName(getPatchParameterValue(line,"AttributeName"));
            filters.setAttributeValue(getPatchParameterValue(line,"AttributeValue"));

            if(this->forceTargetFilesWildcard!=""){
                filesWildcard=this->forceTargetFilesWildcard;
            }
            else{
                filesWildcard=getPatchParameterValue(line,"Files");
            }

            // Check options
            if(xPathExpression.isEmpty() && filters.getElementName().isEmpty()){
                UtilXmlTools::displayErrorMessage("@REMOVE_NODES","ElementName option or XPathExpression option is required.");
            }
            else if(!xPathExpression.isEmpty() && !filters.getElementName().isEmpty()){
                UtilXmlTools::displayErrorMessage("@REMOVE_NODES","ElementName option and XPathExpression option cannot be used simultaneously.");
            }

            if(filters.getAttributeName()!="" && filters.getAttributeValue()==""){
                UtilXmlTools::displayErrorMessage("@REMOVE_NODES","AttributeValue option is required if using AttributeName option.");
            }

            if(filters.getAttributeValue()!="" && filters.getAttributeName()==""){
                UtilXmlTools::displayErrorMessage("@REMOVE_NODES","AttributeName option is required if using AttributeValue option.");
            }

            removeNodesOperation(filters,xPathExpression,filesWildcard);

            filters.clear();
            xPathExpression.clear();
            filesWildcard.clear();
        }
        else if(line.startsWith("@COMMAND")){

            command=GlobalVars::AppExecutable;

            // Append files if forced to
            if(!this->forceTargetFilesWildcard.isEmpty()){
                command+=" --files '"+this->forceTargetFilesWildcard+"' ";
            }

            command+=" "+getPatchParameterValue(line,"Options");

            // Add --no-backups and --no-verbose if patch was called with that arguments
            if(!this->backupsEnabled){
                command.append(" --no-backups ");
            }

            if(!this->verboseEnabled){
                command.append(" --no-verbose ");
            }

            command.replace("'","\""); //replace apostrophe by quotes, to avoid problems
            command.replace("\"\"","'"); // this allow to use '' as ' ('' is transformed in "" and then in ')

            executeCommandOperation(command);

            command.clear();
            filesWildcard.clear();
        }
        else if(line.startsWith("@CUSTOM_CODE")){

            if(this->forceTargetFilesWildcard!=""){
                filesWildcard=this->forceTargetFilesWildcard;
            }
            else{
                filesWildcard=getPatchParameterValue(line,"Files");
            }

            while ( !fileStream.atEnd() && !line.startsWith("</code>")){

                line = fileStream.readLine();

                if(!line.startsWith("<code>") && !line.startsWith("</code>")){
                    jsCode += line + "\n";
                }
            }

            executeCustomCommandOperation(jsCode,filesWildcard);

            jsCode.clear();
            filesWildcard.clear();
        }
    }
}

QString XmlPatch::getPatchParameterValue(const QString& line, QString parameter){

    int startValueIdx, endValueIdx;

    parameter=" "+parameter+" "; // Parameters include a space before and after it's value.

    if(!line.contains(parameter)){
        if(parameter==" ParentElementName " || parameter==" AttributeName " || parameter==" AttributeValue "
                || parameter==" ElementName " || parameter==" XPathExpression "){
            return ""; // not mandatory
        }
        parameter.remove(" "); // just remove the space added so it doesn't look weird when outputted to the user

        UtilXmlTools::displayErrorMessage("Read patch file parameter","Couldn't retrieve '" + parameter + "' parameter.");
    }

    startValueIdx=line.indexOf(parameter); // get the position where parameter is defined
    endValueIdx=line.indexOf("\"",startValueIdx)+1; // get the position where parameter value begins (+1 to ignore mandotory quote)

    startValueIdx=endValueIdx;
    endValueIdx=line.indexOf("\"",startValueIdx); // get the last mandatory quote of the value

    return line.mid(startValueIdx,endValueIdx-startValueIdx); // return the value of the parameter (is in the middle of the mandatory quotes)
}
