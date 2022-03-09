#include "xmltools.h"

// Filters constructor
XmlTools::XmlTools(QString filesWildcard, XmlFilter filter, bool noBackups, bool noVerbose)
{
    this->filesToProcess=UtilXmlTools::getAllXmlFilesByWildcard(filesWildcard);
    this->filters=filter;
    this->backupsEnabled=!noBackups;
    this->verboseEnabled=!noVerbose;

    if(this->filesToProcess.isEmpty()){
        UtilXmlTools::displayErrorMessage("Loading xml files","No XML files were found for the wildcard: "+filesWildcard);
    }
}

// XPath constructor
XmlTools::XmlTools(QString filesWildcard, QString xPathExpression, bool noBackups, bool noVerbose)
{
    this->filesToProcess=UtilXmlTools::getAllXmlFilesByWildcard(filesWildcard);
    this->xPathExpression=xPathExpression;
    this->backupsEnabled=!noBackups;
    this->verboseEnabled=!noVerbose;
}

// Adds new values to an element
void XmlTools::addValues(QString newValues){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){

            QStringList newValuesList, currValuesList;
            QList<pugi::xml_node> elements;

            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled,this->verboseEnabled,"add-values");

            newValuesList=UtilXmlTools::qStringListFromSpacedString(newValues);

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }

            for(int j=0; j<elements.size(); j++){

                currValuesList=UtilXmlTools::qStringListFromSpacedString(elements[j].text().as_string()); // convert each element in a list (uses space as separator)

                for(int k=0; k<newValuesList.size(); k++){
                    if(currValuesList.contains(newValuesList[k])){ // If the current element already contains this value proceed to the next
                        continue;
                    }

                    // // If it doesn't exists yet let's add it
                    if(currValuesList.isEmpty() || (currValuesList.size()==1 && currValuesList[0].isEmpty())){
                        elements[j].text() = QSTR_TO_TEMPORARY_CSTR(newValuesList[k]);
                    }
                    else{
                        elements[j].text() = QSTR_TO_TEMPORARY_CSTR(QString(elements[j].text().as_string()) + " " + newValuesList[k]);
                    }
                }
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document,"add-values");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("add-values", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(), "add-values");

}

void XmlTools::removeValues(QString valuesToRemove){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){

            QList<pugi::xml_node> elements;
            QStringList valuesToRemoveList, currValuesList;
            bool elementChanged=false;

            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled, this->verboseEnabled, "remove-values");

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }

            valuesToRemoveList=UtilXmlTools::qStringListFromSpacedString(valuesToRemove);

            for(int j=0; j<elements.size(); j++){ // O(3)... Optimization may be necessary.
                currValuesList=UtilXmlTools::qStringListFromSpacedString(QString(elements[j].text().as_string())); // convert each element in a list (uses space as separator)

                for(int k=0; k<currValuesList.size(); k++){
                    for(int m=0; m<valuesToRemoveList.size(); m++){
                        if(currValuesList[k]==valuesToRemoveList[m]){
                            currValuesList[k]=""; // flag to deletion
                            elementChanged=true;
                        }
                    }
                }

                if(elementChanged){ // If curr element changed update the XML
                    currValuesList.removeAll(""); // remove all files flagged to deletion
                    elements[j].text()=currValuesList.join(' ').toUtf8().constData();
                    elementChanged=false;
                }
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document, "remove-values");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("remove-values", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(),"remove-values");
}

void XmlTools::replaceValue(QString oldValue, QString newValue){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){

            QList<pugi::xml_node> elements;
            QStringList currValuesList;
            bool elementChanged=false;

            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled, this->verboseEnabled, "replace-value");

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }

            for(int j=0; j<elements.size(); j++){
                currValuesList=UtilXmlTools::qStringListFromSpacedString(QString(elements[j].text().as_string())); // convert each element in a list (uses space as separator)

                for(int k=0; k<currValuesList.size(); k++){
                    if(currValuesList[k]==oldValue){ // Found a match with the old value?
                        currValuesList[k]=newValue; // If found replace it with the new value
                        elementChanged=true;
                    }
                }

                if(elementChanged){ // If curr element changed update the XML
                    elements[j].text()=currValuesList.join(" ").toUtf8().constData();
                }
                elementChanged=false;
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document, "replace-value");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("replace-value", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(), "replace-value");
}

// Replaces all current values of an element by another (can be specified only specific positions)
void XmlTools::replaceAll(QString value, QString valuePositions){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){

            QList<pugi::xml_node> elements;

            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled, this->verboseEnabled, "replace-all");

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }


            // Let's start the override
            for(int j=0; j<elements.size(); j++){
                if(valuePositions!=""){
                    elements[j].text()= QSTR_TO_TEMPORARY_CSTR(replaceSpecificPositions(value, QString(elements[j].text().as_string()), valuePositions));
                }
                else{
                    elements[j].text()=value.toUtf8().constData();
                }
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document, "replace-all");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("replace-all", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(), "replace-all");
}

// Update a set of XML elements values based in the difference between the old and new value
// This can be used in multiple files if the difference between one file and other are the same e.g. increment to all current object positions 100 in y (A difference of -100).
// E.g. oldValue=5 , newValue=7; diffBetweenOldAndNewValue=-2
void XmlTools::updateElements(QString diffBetweenOldAndNewValue){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){

            QList<pugi::xml_node> elements;
            MultiDimVar lastXmlValue(0); // inicialize with any value or dimension
            MultiDimVar currXmlValue(0);
            MultiDimVar newXmlValue(0); // value that will update currValue

            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled,this->verboseEnabled, "update-elements");

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }

            if(elements.size() > 0){

                lastXmlValue=MultiDimVar(QString(elements[0].text().as_string())); // the lastXmlValue will begin to be the first one of the node
                newXmlValue=MultiDimVar::sub(lastXmlValue, MultiDimVar(diffBetweenOldAndNewValue));
                elements[0].text() = newXmlValue.toString().toUtf8().constData(); // update the first element with the new one already

                // If we have more than 1 element
                if(elements.size()>1){
                    currXmlValue=MultiDimVar(QString(elements[1].text().as_string())); // the currXmlValue will begin to be the second one of the node

                    // Let's start the node update
                    for(int j=1; j<elements.size()-1; j++){ // We start in 1 because the 0 is already saved in lastXmlValue // -1 because we will also work with the next one in the current

                        newXmlValue=MultiDimVar::sum(newXmlValue,MultiDimVar::sub(currXmlValue,lastXmlValue));
                        elements[j].text() = newXmlValue.toString().toUtf8().constData(); // update element with the new value
                        lastXmlValue=currXmlValue;
                        currXmlValue=MultiDimVar(QString(elements[j+1].text().as_string()));

                    }

                    // To update too last element (avoid out of bound because i+1)
                    newXmlValue=MultiDimVar::sum(newXmlValue,MultiDimVar::sub(currXmlValue,lastXmlValue));
                    elements[elements.size()-1].text() = newXmlValue.toString().toUtf8().constData(); // update element with the new value
                }
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document, "update-elements");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("update-elements", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(),"update-elements");

}

// Invert a set of XML elements with specified name (and optionally a parent name)
void XmlTools::invertElements(){

    try{
        // Process all XmlFiles
        for(int i=0; i<this->filesToProcess.size(); i++){
            UtilXmlTools::loadXmlFile(this->filesToProcess[i],this->document,this->rootNode,this->backupsEnabled, this->verboseEnabled, "invert-elements");

            QList<pugi::xml_node> elements;
            QStringList invertedElements; //Inverting the element order

            // Check how the elements will be fetched via element name or xpath expression
            if(this->xPathExpression==""){
                UtilXmlTools::getAllNamedElements(this->rootNode,elements,this->filters);
            }
            else{
                UtilXmlTools::getAllXpathElements(this->xPathExpression,this->document,elements);
            }

            // Read all elements and save to the list
            for(int j=elements.size()-1; j>=0; j--){
                invertedElements << QString(elements[j].text().as_string());
            }

            // Override the tree with the inverted order
            for(int j=0; j<elements.size(); j++){
                elements[j].text()= invertedElements[j].toUtf8().constData();
            }

            UtilXmlTools::saveXmlFile(this->filesToProcess[i],this->document, "invert-elements");
        }
    }
    catch(const std::exception &e){
        UtilXmlTools::displayErrorMessage("invert-elements", QString("An exception was caught: ") + e.what());
    }

    UtilXmlTools::displaySuccessMessage(this->filesToProcess.size(),"invert-elements");

}

// Replaces specific values (given the position) for a new value
// [ currValues / positionsToReplaced are strings with composed by another strings space separated ]
// Returns a new string (space separated) will values replaced
QString XmlTools::replaceSpecificPositions(const QString &newValue, const QString &currValues, const QString &positionsToReplace){

    QList<int> positionsToReplaceList;
    QStringList currValuesList;

    positionsToReplaceList=UtilXmlTools::qListIntFromSpacedString(positionsToReplace);
    currValuesList=UtilXmlTools::qStringListFromSpacedString(currValues);

    // Make some validation before the replacing
    if(currValuesList.size()<positionsToReplaceList.size()){
        UtilXmlTools::displayErrorMessage("replaceSpecificPositions","There are more positions to replace than the current xml values.");
    }

    foreach(int pos, positionsToReplaceList){
        if(pos>currValuesList.size()-1 || pos < 0){ //Are positions valid for the current values? //-1 because starts at 0
            UtilXmlTools::displayErrorMessage("replaceSpecificPositions","One or more of the specified positions to replace are out of range.");
        }
    }
    //

    // Finally replace the specified values
    foreach(int pos, positionsToReplaceList){
        currValuesList[pos]=newValue;
    }

    return currValuesList.join(" ");

}
