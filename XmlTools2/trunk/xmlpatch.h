#ifndef XMLPATCH_H
#define XMLPATCH_H

#include "xmltools.h"
#include "xmlcustomcode.h"
#include "optionsparser.h"

class XmlPatch
{
public:
    XmlPatch(QString patchFilesWildcard, QString forceTargetFilesWildcard, bool noBackups, bool noVerbose);
    void readAndProcessPatchFile();
private:
    QVector<QString> patchFilesToProcess;
    QString forceTargetFilesWildcard;
    pugi::xml_document document;
    pugi::xml_node rootNode;
    bool backupsEnabled, verboseEnabled;
    XmlCustomCode *customCodeOperation;
    QString getPatchParameterValue(const QString& line, QString parameter);
    void insertNodesOperation(const QString &xmlString, XmlFilter &filters, const QString &xPathExpression, const QString &filesWildcard="");
    void removeNodesOperation(XmlFilter &filters, const QString &xPathExpression, const QString &filesWildcard="");
    void executeCommandOperation(const QString &commandString);
    void executeCustomCommandOperation(const QString &jsString, const QString &filesWildcard="");
    void checkPatchVersion(const QString &file, QTextStream &fileStream);
    void checkAndProcessValidCommands(QTextStream &fileStream);
};

#endif // XMLPATCH_H
