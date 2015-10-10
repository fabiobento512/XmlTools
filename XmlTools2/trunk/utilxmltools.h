#ifndef UTILXMLTOOLS_H
#define UTILXMLTOOLS_H

// Utilities functions specific used for Xml Tools (not general functions)

#include "util.h"
#include "xmlfilter.h"
#include "libs/pugixml.hpp"

namespace UtilXmlTools{

QVector<QString> getAllXmlFilesByWildcard(const QString &wildcard);
QVector<QString> getAllPatchFilesByWildcard(const QString &wildcard);
void backupFile(const QString &file, bool verboseEnabled);
void getAllNamedElements(pugi::xml_node &node, QList<pugi::xml_node> &result, XmlFilter &filters);
void getAllXpathElements(const QString &xPathExpression, pugi::xml_document &doc, QList<pugi::xml_node> &result);
void displaySuccessMessage(const int numberOperations, const QString &operation);
void displayErrorMessage(const QString& operation, const QString &message, bool exitProgram=true);
pugi::xml_node getFirstNamedElement(pugi::xml_node &node, XmlFilter &filters);
pugi::xml_node getFirstXpathElement(const QString &xPathExpression, pugi::xml_document &doc);

//// inline functions

inline void loadXmlFile(const QString &file, pugi::xml_document &document, pugi::xml_node &rootNode, bool backupsEnabled, bool verboseEnabled, const QString &operationForErrorMessage){

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

inline void saveXmlFile(const QString &file, pugi::xml_document &document, const QString &operationForErrorMessage){
    if(!document.save_file(file.toUtf8().constData(), "\t", pugi::format_indent | pugi::format_save_file_text | pugi::format_no_escapes)){ // output as the system new lines ending
        UtilXmlTools::displayErrorMessage(operationForErrorMessage,"An error ocurred while saving '" + file + "' XML file");
    }
}

//inline void checkIfValidXmlFile(const QString &file){
//    if(!file.endsWith(".xml",Qt::CaseInsensitive)){
//        std::cout << "Tried to load a non xml file: '" << Util::toCstr(file) << "'. XmlTools only work over XmlFiles." << std::endl;
//        exit(1);
//    }
//}

}

#endif // UTILXMLTOOLS_H
