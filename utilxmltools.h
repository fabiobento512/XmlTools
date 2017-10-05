#ifndef UTILXMLTOOLS_H
#define UTILXMLTOOLS_H

// Utilities functions specific used for Xml Tools (not general functions)

#include "util.h"
#include "xmlfilter.h"
#include <pugixml/pugixml.hpp>

#include <iostream>

namespace UtilXmlTools{

QVector<QString> getAllXmlFilesByWildcard(const QString &wildcard);
QVector<QString> getAllPatchFilesByWildcard(const QString &wildcard);
void backupFile(const QString &file, bool verboseEnabled);
void getAllNamedElements(pugi::xml_node &node, QList<pugi::xml_node> &result, XmlFilter &filters);
void getAllXpathElements(const QString &xPathExpression, pugi::xml_document &doc, QList<pugi::xml_node> &result);
void displaySuccessMessage(const int numberOperations, const QString &operation);
void displayErrorMessage(const QString& operation, const QString &message, bool exitProgram=true);
void loadXmlFile(const QString &file, pugi::xml_document &document, pugi::xml_node &rootNode, bool backupsEnabled, bool verboseEnabled, const QString &operationForErrorMessage);
void saveXmlFile(const QString &file, pugi::xml_document &document, const QString &operationForErrorMessage);
pugi::xml_node getFirstNamedElement(pugi::xml_node &node, XmlFilter &filters);
pugi::xml_node getFirstXpathElement(const QString &xPathExpression, pugi::xml_document &doc);
QStringList qStringListFromSpacedString(const QString &mySpacedString);
QStringList QStringToArgsArray(const QString &args);
QStringList getAllFilesByWildcard(const QString &wildcard);
QList<int> qListIntFromSpacedString(const QString &mySpacedString);
QList<double> qListDoubleFromSpacedString(const QString &mySpacedString);

}

#endif // UTILXMLTOOLS_H
