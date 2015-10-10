#include "utilxmltools.h"

namespace UtilXmlTools{

// As this will not likely be modified we use QVector instead of QList since it is more cache friendly
QVector<QString> getAllXmlFilesByWildcard(const QString &wildcard){
    QStringList validFilesMatching;
    QStringList filesMatching;

    // Get all files matching the wildcard

    filesMatching=Util::getAllFilesByWildcard(wildcard);

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

    filesMatching=Util::getAllFilesByWildcard(wildcard);

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
        if(!Util::backupFile(file)){
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
                    Util::toQString((*currNode).attribute(filters.getAttributeName().toUtf8().constData()).value()) == filters.getAttributeValue()) ){ // Seems node attribute must be converted to qtsring to remove \r\n needs to check in future!

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
                    Util::toQString((*currNode).attribute(filters.getAttributeName().toUtf8().constData()).value()) == filters.getAttributeValue()) ){
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

}
