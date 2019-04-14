#include "Mainwindow.h"
#include "QLogger.h"
#include "IntegrationTests.h"

#define ENABLE_TEST 0

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    app.installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    app.installTranslator(&AssetsOptTranslator);

#if ENABLE_TEST
    IntegrationTests tests(QCoreApplication::arguments().at(1));
    tests.runAllTests();
#else
    MainWindow w;
    w.show();
#endif

    return app.exec();
}
