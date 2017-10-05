#include "optionsparser.h"

OptionsParser::OptionsParser(QStringList args)
{
    this->args=args;
}

void OptionsParser::parse(){

    QCommandLineParser parser;
    parser.setApplicationDescription("Additional documentation can be found at: http://wiki.oni2.net/XmlTools");

    std::unique_ptr<XmlTools> myXmlTools;
    QString filesWildCard, patchFilesWildCard, forceTargetFilesWildcard;
    QString currentVal, newVal, diffOldNewVal, positions;
    QString xPathExpression;
    XmlFilter filters; // Filters

    bool noBackups=false;
    bool noVerbose=false;

    QCommandLineOption addValuesOption(QStringList() << "a" << "add-values", "Add values to a set of XML elements.");
    QCommandLineOption removeValuesOption(QStringList() << "remove-values", "Removes values from a set of XML elements.");
    QCommandLineOption replaceValueOption(QStringList() << "replace-value", "Replaces 1 value in a set of XML elements.");
    QCommandLineOption replaceAllValuesOption(QStringList() << "replace-all-values", "Replaces all values in a set of XML elements.");
    QCommandLineOption updateElementsOption(QStringList() << "u" << "update-elements", "Updates all values in a set of XML elements.");
    QCommandLineOption invertElementsOption(QStringList() << "i" << "invert-elements", "Inverts a set of XML elements.");

    QCommandLineOption currentValOption(QStringList() << "c" << "current-val", "Current value(s) [use space as separator].","current-val");
    QCommandLineOption newValOption(QStringList() << "n" << "new-val", "New value(s) [use space as separator].","new-val");
    QCommandLineOption diffOldNewValueOption(QStringList() << "d" << "diff-old-new-val", "Difference between old and new value.","diff-old-new-val");
    QCommandLineOption positionsValueOption(QStringList() << "positions", "Positions [use space as separator] [0-index based].","positions");

    QCommandLineOption filesOption(QStringList() << "f" << "files", "XML files to process [wildcards supported].", "files");
    QCommandLineOption patchFilesOption(QStringList() << "p" << "patch-files" , "Patch files to process [wildcards supported].", "patch-files");
    QCommandLineOption forceTargetFilesOption(QStringList() << "force-target-files" , "Force the patch-files operation in the specified XML files. [wildcards supported].", "force-target-files");
    QCommandLineOption elementNameOption(QStringList() << "e" << "element-name", "Name of the XML element(s) where processing will occur.", "element-name");
    QCommandLineOption parentElementNameOption(QStringList() << "parent-element-name", "Name of the XML parent element of <element-name> [used as filter].", "parent-element-name");
    QCommandLineOption attributeNameOption("attribute-name", "Attribute name of <element-name>  [used as filter].", "attribute-name");
    QCommandLineOption attributeValueOption("attribute-value", "Attribute value of <attribute-name>  [used as filter].", "attribute-value");
    QCommandLineOption xPathExpressionOption(QStringList() << "x" << "xpath-expression", "XPath 1.0 expression to select elements where processing will occur.", "xpath-expression");
    QCommandLineOption noBackupsOption(QStringList()  << "no-backups", "No backups [faster processing].");
    QCommandLineOption noVerboseOption(QStringList()  << "no-verbose", "Reduce the number of text messages output [faster processing].");
    QCommandLineOption aeiPatchFilesListOption(QStringList() << "aei-patch-files-list" , "Exclusive option for AEI. Provide a list of patches to process.", "aei-patch-files-list");

    parser.addOption(addValuesOption);
    parser.addOption(removeValuesOption);
    parser.addOption(replaceValueOption);
    parser.addOption(replaceAllValuesOption);
    parser.addOption(updateElementsOption);
    parser.addOption(invertElementsOption);

    parser.addOption(currentValOption);
    parser.addOption(newValOption);
    parser.addOption(diffOldNewValueOption);
    parser.addOption(positionsValueOption);

    parser.addOption(filesOption);
    parser.addOption(patchFilesOption);
    parser.addOption(forceTargetFilesOption);
    parser.addOption(elementNameOption);
    parser.addOption(parentElementNameOption);
    parser.addOption(attributeNameOption);
    parser.addOption(attributeValueOption);
    parser.addOption(xPathExpressionOption);
    parser.addOption(noBackupsOption);
    parser.addOption(noVerboseOption);
    parser.addOption(aeiPatchFilesListOption);

    parser.addVersionOption();
    parser.addHelpOption();

    // Process the actual command line arguments given by the user
    parser.process(this->args);

    // If no arguments show help option
    if(this->args.size()==1){
        parser.showHelp();
    }

    // Begin by processing AEI special parameter
    if(parser.isSet(aeiPatchFilesListOption)){

        QFile inputFile(parser.value(aeiPatchFilesListOption));
        QStringList temp; // contains the complete line. first in
        QStringList patchFiles, targetXmlFiles;
        QString line;

        if (inputFile.open(QIODevice::ReadOnly))
        {

            QTextStream fileStream(&inputFile);

            // Process all the patches until the end of file
            while ( !fileStream.atEnd() ){
                line = fileStream.readLine();

                if(line.startsWith('"')){ // Only read when starting with quotes
                    temp=line.split('"',QString::SkipEmptyParts); // We need to use more than space, because if paths contains spaces...
                    patchFiles << temp[0];
                    targetXmlFiles << temp[2]; // space is in idx 1
                }
            }

            inputFile.close();

            // Now let's process each patch file and target file
            for(int i=0; i<patchFiles.size(); i++){
                XmlPatch myXmlPatch(patchFiles[i],targetXmlFiles[i],true,true); // use --no-backups and --no-verbose for AEI
                myXmlPatch.readAndProcessPatchFile(); // process current file
            }

            UtilXmlTools::displaySuccessMessage(patchFiles.size(),"AEI patches");

        }
        else{
            UtilXmlTools::displayErrorMessage("Read file", "Error opening AEI-patch-files-list file: '" + parser.value(aeiPatchFilesListOption) + "'.\n" + inputFile.errorString());
        }

        return;
    }


    // Check if the user doesn't want backups (it boosts XmlTools peformance, lower disk output)
    if(parser.isSet(noBackupsOption)){
        noBackups=true;
    }

    // Check if the user doesn't want verbose mode (it boosts XmlTools peformance, lower std output)
    if(parser.isSet(noVerboseOption)){
        noVerbose=true;
    }

    // Get patch files wildcard if available
    if(parser.isSet(patchFilesOption)){
        patchFilesWildCard=parser.value(patchFilesOption);

        // Never reached
        //        if(patchFilesWildCard==""){
        //            UtilXmlTools::displayErrorMessage("Parameter Parsing", "patch-files option requires 1 value: <patch-files> the patch files to process (wildcards supported).");
        //        }

        forceTargetFilesWildcard=parser.value(forceTargetFilesOption);

        XmlPatch myXmlPatch(patchFilesWildCard,forceTargetFilesWildcard,noBackups,noVerbose);
        myXmlPatch.readAndProcessPatchFile(); // beging file patch processing

        return;
    }

    if(!parser.isSet(elementNameOption) && !parser.isSet(xPathExpressionOption)){
        UtilXmlTools::displayErrorMessage("Parameter Parsing","element-name option or xpath-expression option is required if not using patch-files option.");
    }
    else if(parser.isSet(elementNameOption) && parser.isSet(xPathExpressionOption)){
        UtilXmlTools::displayErrorMessage("Parameter Parsing","element-name option and xpath-expression options cannot be used simultaneously.");
    }

    // Get element name if available
    if(parser.isSet(elementNameOption)){
        filters.setElementName(parser.value(elementNameOption));
    }

    // Get xpath expression if available
    if(parser.isSet(xPathExpressionOption)){
        xPathExpression=parser.value(xPathExpressionOption);
    }

    // Get current value(s) if avaialabe
    if(parser.isSet(currentValOption)){
        currentVal=parser.value(currentValOption);
    }

    // Get new value(s) if avaialabe
    if(parser.isSet(newValOption)){
        newVal=parser.value(newValOption);
    }

    // Get difference between old and new value if avaialabe
    if(parser.isSet(diffOldNewValueOption)){
        diffOldNewVal=parser.value(diffOldNewValueOption);
    }

    // Get positions if avaialabe
    if(parser.isSet(positionsValueOption)){
        positions=parser.value(positionsValueOption);
    }

    // Get parent element name if available
    if(parser.isSet(parentElementNameOption)){
        filters.setParentElementName(parser.value(parentElementNameOption));
    }

    // Get attribute name if available
    if(parser.isSet(attributeNameOption)){
        filters.setAttributeName(parser.value(attributeNameOption));
    }

    // Get attribute value if available
    if(parser.isSet(attributeValueOption)){
        filters.setAttributeValue(parser.value(attributeValueOption));
    }

    // Check attribute filters
    if(filters.getAttributeName()!="" && filters.getAttributeValue()==""){
        UtilXmlTools::displayErrorMessage("Parameter Parsing","attribute-value option is required if using attribute-name option.");
    }

    if(filters.getAttributeValue()!="" && filters.getAttributeName()==""){
        UtilXmlTools::displayErrorMessage("Parameter Parsing","attribute-name option is required if using attribute-value option.");
    }

    // Get files wildcard if available
    if(parser.isSet(filesOption)){
        filesWildCard=parser.value(filesOption);
    }
    else{
        UtilXmlTools::displayErrorMessage("Parameter Parsing", "files option requires 1 value: <files> the XML files to process (wildcards supported).");
    }

    if(parser.isSet(elementNameOption)){
        myXmlTools = std::make_unique<XmlTools>(filesWildCard,filters,noBackups,noVerbose);
    }
    else{
        myXmlTools = std::make_unique<XmlTools>(filesWildCard,xPathExpression,noBackups,noVerbose);
    }


    // Users wants an add-option?
    if(parser.isSet(addValuesOption)){

        if(newVal==""){
            UtilXmlTools::displayErrorMessage("Parameter Parsing", "add-value option requires 1 value: <new-val> the new values to add (space separated).");
        }

        myXmlTools->addValues(newVal);
    }
    else if(parser.isSet(removeValuesOption)){ // Or remove-values option?

        if(currentVal==""){
            UtilXmlTools::displayErrorMessage("Parameter Parsing","remove-values option requires 1 value: <current-val> the current values to remove (space separated).");
        }

        myXmlTools->removeValues(currentVal);
    }
    else if(parser.isSet(replaceValueOption)){ // Or replace-value option?

        if(currentVal=="" || newVal==""){
            UtilXmlTools::displayErrorMessage("Parameter Parsing","replace-value option requires 2 values: <current-val> the current value and <new-val> the new value.");
        }

        myXmlTools->replaceValue(currentVal,newVal);
    }
    else if(parser.isSet(replaceAllValuesOption)){ // Or replace-all-values option?

        if(newVal=="" && positions==""){
            UtilXmlTools::displayErrorMessage("Parameter Parsing","replace-all-values option requires 1 value: <new-val> the new value.\n" +
                                              QString("It has also 1 optional value: <positions> the positions to replace (space separated and 0-index based)."));
        }

        myXmlTools->replaceAll(newVal,positions);
    }
    else if(parser.isSet(updateElementsOption)){ // Or update-elements option?

        if(diffOldNewVal==""){
            UtilXmlTools::displayErrorMessage("Parameter Parsing","update-elements option requires 1 value: <diff-old-new-val> the difference between one current element "+
                                              QString("value and one new element value (current value and new value must have the same position)."));
        }

        myXmlTools->updateElements(diffOldNewVal);
    }
    else if(parser.isSet(invertElementsOption)){ // Or invert-elements option?
        myXmlTools->invertElements();
    }
    else{
        UtilXmlTools::displayErrorMessage("Parameter Parsing","XmlTools needs an operation to perform. Possible operations are:\n"+
                                          QString("--patch-files\n--add-values\n--remove-values\n--replace-value\n--replace-all-values\n--update-elements\n--invert-elements"));
    }
}
