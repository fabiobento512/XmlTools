#include "utilxmltools.h"

namespace UtilXmlTools{

// As this will not likely be modified we use QVector instead of QList since it is more cache friendly
QVector<QString> getAllXmlFilesByWildcard(const QString &wildcard){
    QStringList validFilesMatching;
    QStringList filesMatching;

    // Get all files matching the wildcard

    filesMatching=UtilXmlTools::getAllFilesByWildcard(wildcard);

    // Check if all are XmlFiles, only return valid XmlFiles

    for(int i=0; i<filesMatching.size(); i++){
        if(filesMatching[i].endsWith(".xml",Qt::CaseInsensitive)){
            validFilesMatching << filesMatching[i];
        }
    }

    return validFilesMatching.toVector();

}

QVector<QString> getAllPatchFilesByWildcard(const QString &wildcard){
    QStringList validFilesMatching;
    QStringList filesMatching;

    // Get all files matching the wildcard

    filesMatching=UtilXmlTools::getAllFilesByWildcard(wildcard);

    // Check if all are PatchFiles, only return valid PatchFiles

    for(int i=0; i<filesMatching.size(); i++){
        if(filesMatching[i].endsWith(".patch",Qt::CaseInsensitive) || filesMatching[i].endsWith(".oni-patch",Qt::CaseInsensitive)){
            validFilesMatching << filesMatching[i];
        }
    }

    return validFilesMatching.toVector();

}

void backupFile(const QString &file, bool verboseEnabled){
    if(!QFile::exists(file+".bak")){
        if(!Util::FileSystem::backupFile(file)){
            std::cerr << "Couldn't back up file '" << file.toUtf8().constData() << "'. Aborting." << std::endl;
            exit(1);
        }
    }
    else{
        if(verboseEnabled){
            std::cout << "Backup file '" << file.toUtf8().constData() << "'' already exists. Skipping..." << std::endl;
        }
    }
}

void getAllXpathElements(const QString &xPathExpression, pugi::xml_document &doc, QList<pugi::xml_node> &result){

    pugi::xpath_node_set selectedNodes;
    pugi::xpath_node node;

    try
    {
        selectedNodes = doc.select_nodes(xPathExpression.toUtf8().constData());
    }

    catch (const pugi::xpath_exception& e)
    {
        displayErrorMessage("XPath element selection","Selection of elements using the XPathExpression: '" + xPathExpression + "' failed:\n" + e.what());
    }

    for (pugi::xpath_node_set::const_iterator currNode = selectedNodes.begin(); currNode != selectedNodes.end(); ++currNode)
    {
        node = *currNode;
        if(node){ // if node != null
            result << node.node();
        }
    }

    if(result.isEmpty()){
        result << pugi::xml_node(); // add an empty node if none found
    }

}

pugi::xml_node getFirstXpathElement(const QString &xPathExpression, pugi::xml_document &doc){
    pugi::xpath_node selectedNode;

    try
    {
        selectedNode = doc.select_single_node(xPathExpression.toUtf8().constData());
    }

    catch (const pugi::xpath_exception& e)
    {
        displayErrorMessage("XPath element selection","Selection of element using the XPathExpression: '" + xPathExpression + "' failed:\n" + e.what());
    }

    return selectedNode.node();
}

void getAllNamedElements(pugi::xml_node &node, QList<pugi::xml_node> &result, XmlFilter &filters){
    for (pugi::xml_node_iterator currNode = node.begin(); currNode != node.end(); ++currNode)
    {

        if ((*currNode).name() == filters.getElementName() && (filters.getParentElementName() == "" || filters.getParentElementName() == (*currNode).parent().name())
                && (filters.getAttributeName() == "" ||
                    QString((*currNode).attribute(filters.getAttributeName().toUtf8().constData()).value()) == filters.getAttributeValue()) ){ // Seems node attribute must be converted to qtsring to remove \r\n needs to check in future!

            result << *currNode;
            continue;
        }
        getAllNamedElements(*currNode,result,filters);
    }
}

pugi::xml_node getFirstNamedElement(pugi::xml_node &node, XmlFilter &filters){

    pugi::xml_node foundNode;

    for (pugi::xml_node_iterator currNode = node.begin(); currNode != node.end(); ++currNode)
    {
        if ((*currNode).name() == filters.getElementName() && (filters.getParentElementName() == "" || filters.getParentElementName() == (*currNode).parent().name())
                && (filters.getAttributeName() == "" ||
                    QString((*currNode).attribute(filters.getAttributeName().toUtf8().constData()).value()) == filters.getAttributeValue()) ){
            return *currNode;
        }

        foundNode=getFirstNamedElement(*currNode,filters);

        if(foundNode.type()!=pugi::node_null){
            return foundNode;
        }

    }

    return foundNode;
}

void displaySuccessMessage(const int numberOperations, const QString &operation){
    std::cout << "------------------------------------------------------------------------" << std::endl;
    std::cout << numberOperations << " " << operation.toUtf8().constData() << " operation(s) completed with success!" << std::endl;
    std::cout << "------------------------------------------------------------------------" << std::endl;
}

void displayErrorMessage(const QString& operation, const QString &message, bool exitProgram){
    std::cerr << "************************************************************************" << std::endl;
    std::cerr << operation.toUtf8().constData() << " operation failed!" << std::endl << std::endl;
    std::cerr << message.toUtf8().constData() << std::endl << std::endl;
    if(exitProgram) std::cerr << "Aborting..." << std::endl;
    std::cerr << "************************************************************************" << std::endl;
    if(exitProgram) exit(1);
}

QStringList qStringListFromSpacedString(const QString &mySpacedString){
    return mySpacedString.split(" ");
}

QList<int> qListIntFromSpacedString(const QString &mySpacedString){
    QStringList stringList;
    QList<int> intList;

    stringList = mySpacedString.split(" ");

    foreach(QString value, stringList){
        bool ok;

        intList << value.toInt(&ok);

        if(!ok){
            throw std::runtime_error(QString("Impossible to convert string '" + value + "' to int!").toUtf8().constData());
        }
    }

    return intList;
}

QList<double> qListDoubleFromSpacedString(const QString &mySpacedString){
    QStringList stringList;
    QList<double> doubleList;

    stringList = mySpacedString.split(" ");

    foreach(QString value, stringList){
        bool ok;

        doubleList << value.toDouble(&ok);

        if(!ok){
            throw std::runtime_error(QString("Impossible to convert string '" + value + "' to double!").toUtf8().constData());
        }
    }

    return doubleList;
}

void loadXmlFile(const QString &file, pugi::xml_document &document, pugi::xml_node &rootNode, bool backupsEnabled, bool verboseEnabled, const QString &operationForErrorMessage){

    pugi::xml_parse_result result = document.load_file(file.toUtf8().constData());
    rootNode=document.root();

    if(result.status==pugi::status_ok){
        if(verboseEnabled){
            std::cout << "File '" << file.toUtf8().constData() << "' loaded with sucess." << std::endl;
        }
    }
    else{
        UtilXmlTools::displayErrorMessage(operationForErrorMessage,"An error ocurred while loading '" +file + "' XML file\n" + result.description());
    }

    if(backupsEnabled){
        UtilXmlTools::backupFile(file,verboseEnabled); // bake a backup of the file.
    }

}

void saveXmlFile(const QString &file, pugi::xml_document &document, const QString &operationForErrorMessage){
    if(!document.save_file(file.toUtf8().constData(), "\t", pugi::format_indent | pugi::format_save_file_text | pugi::format_no_escapes)){ // output as the system new lines ending
        UtilXmlTools::displayErrorMessage(operationForErrorMessage,"An error ocurred while saving '" + file + "' XML file");
    }
}

//TODO Needs optimization
QStringList QStringToArgsArray(const QString &args){
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

// TODO: Replace later with a more generic function and put in util library
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

    pathNormalized=Util::FileSystem::normalizePath(wildcard); // Convert slashes to work in both mac and windows

    if(pathNormalized.contains("/")){ // If contains full path
        endOfPathIdx=pathNormalized.lastIndexOf("/"); // get last slash

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
