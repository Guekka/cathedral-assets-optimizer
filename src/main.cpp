#include "Mainwindow.h"
#include "QLogger.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    app.installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    app.installTranslator(&AssetsOptTranslator);

    QLogger::QLoggerManager::getInstance();

    MainWindow w;
    w.show();

    return app.exec();
}
