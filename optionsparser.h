#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include "xmlpatch.h"

#include <QCommandLineOption>
#include <QCommandLineParser>

class OptionsParser
{
public:
    OptionsParser(QStringList args);
    void parse();
private:
    QStringList args;
};

#endif // OPTIONSPARSER_H
