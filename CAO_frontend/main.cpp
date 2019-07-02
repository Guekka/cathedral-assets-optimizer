#include "pch.h"
#include "Custom.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&AssetsOptTranslator);

    try{
        Custom win;
        win.show();
    }
    catch(const std::exception& e)
    {
        QMessageBox box;
        box.setIcon(QMessageBox::Critical);
        box.setText("An exception has been encountered and the app was forced to stop: " + QString(e.what()));
        box.exec();
    }

    return app.exec();
}
