#ifndef XMLTOOLS_H
#define XMLTOOLS_H


#include <string>
#include <stdio.h>

#include "utilxmltools.h"
#include "multidimvar.h"

// Template got from here:
// http://www.lubby.org/ebooks/qtconsoleapp2/qtconsoleapp2.html

// The xml library used was pugixml:
// https://code.google.com/p/pugixml/

class XmlTools
{
public:
    XmlTools(QString filesWildcard, XmlFilter filter, bool noBackups, bool noVerbose);
    XmlTools(QString filesWildcard, QString xPathExpression, bool noBackups, bool noVerbose);
    void addValues(QString newValues);
    void removeValues(QString valuesToRemove);
    void replaceValue(QString oldValue, QString newValue);
    void replaceAll(QString value, QString valuePositions = "");
    void updateElements(QString diffBetweenOldAndNewValue);
    void invertElements();
private:
    QString replaceSpecificPositions(const QString &newValue, const QString &currValues, const QString &positionsToReplace);
    pugi::xml_document document;
    pugi::xml_node rootNode;
    QVector<QString> filesToProcess;
    QString xPathExpression;
    XmlFilter filters;
    bool backupsEnabled, verboseEnabled;
};

#endif // XMLTOOLS_H
