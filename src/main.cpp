#include "Mainwindow.h"
#include "QLogger.h"
#include "windows.h"
#include "IntegrationTests.h"
#include <QCommandLineParser>

#define ENABLE_TEST 0

bool parseArguments()
{
    QCommandLineParser parser;

    parser.addHelpOption();
    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");

    parser.addOptions({
                          {"dr", "Enables dry run"},
                          {"l", "Log level: from 0 (maximum) to 6", "value", "0"},
                          {"m", "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, 2 for medium optimization, 3 for full optimization.", "value", "0"},
                          {"t", "Texture processing level: 0 (default) to disable optimization, 1 for necessary optimization, 2 for full optimization.", "value", "0"},
                          {"a", "Enables animations processing"},
                          {"be", "Enables bsa extraction."},
                          {"bc", "Enables bsa creation."},
                          {"bl", "Enables bsa packing of loose files."},
                          {"bd", "Enables deletion of bsa backups."},
                          {"bs", "Enables splitting assets."},
                      });

    parser.process(QCoreApplication::arguments());

    MainOptimizer::resetSettings();

    QSettings settings("Cathedral Assets Optimizer.ini", QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, "Cathedral Assets Optimizer.ini");

    QString path = QDir::cleanPath(parser.positionalArguments().at(0));
    QString mode = parser.positionalArguments().at(1);

    if(!QDir(path).exists())
    {
        QTextStream(stderr) << "\nError. This path does not exist.";
        return false;
    }
    settings.setValue("SelectedPath", path);

    if(mode == "om")
        settings.setValue("iMode", 0);
    else if(mode == "sm")
        settings.setValue("iMode", 1);
    else
    {
        QTextStream(stderr) << "\nError. This mode does not exist.";
        return false;
    }


    settings.setValue("bDryRun", parser.isSet("dr"));

    if(parser.isSet("l"))
        settings.setValue("iLogLevel", parser.value("l").toInt());

    if(parser.isSet("m"))
        settings.setValue("Meshes/iMeshesOptimizationLevel", parser.value("m").toInt());

    if(parser.isSet("t"))
        settings.setValue("Textures/iTexturesOptimizationLevel", parser.value("t").toInt());

    settings.setValue("Animations/bAnimationsOptimization", parser.isSet("a"));

    settings.beginGroup("BSA");
    settings.setValue("bBsaExtract", parser.isSet("be"));
    settings.setValue("bBsaCreate", parser.isSet("bc"));
    settings.setValue("bBsaPackLooseFiles", parser.isSet("bl"));
    settings.setValue("bBsaDeleteBackup", parser.isSet("bd"));
    settings.setValue("bBsaSplitAssets", parser.isSet("bs"));
    settings.endGroup();

    return true;
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    app.installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    app.installTranslator(&AssetsOptTranslator);

    //If tests are enabled, run tests instead of running standard process

#if ENABLE_TEST
    IntegrationTests tests(QCoreApplication::arguments().at(1));
    tests.runAllTests();

#else


#ifdef _WIN32

    MainWindow w;

//#ifndef QT_DEBUG
    //If ran from a console, using this console instead of opening the GUI.
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        if (parseArguments())
        {
            MainOptimizer optimizer;
            return 1 ; //optimizer.mainProcess();
        }
        else
            return 1;
    }
    else
//#endif
        w.show();

#endif //_WIN32

#endif // ELSE ENABLE_TEST

    return app.exec();
}
