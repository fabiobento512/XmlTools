#include "main.h"

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    // Set application parameters
    QCoreApplication::setApplicationName(GlobalVars::AppName);
    QCoreApplication::setApplicationVersion(GlobalVars::AppVersion);

    OptionsParser myParser(app.arguments());
    myParser.parse();

    //app.exec();

    return 0;
}
