#include "pch.h"
#include "SkyrimSE.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&AssetsOptTranslator);

    SkyrimSE sse;
    sse.show();

    return app.exec();
}
