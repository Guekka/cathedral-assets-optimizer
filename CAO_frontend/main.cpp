#include "pch.h"
#include "SkyrimSE.h"
#include "Skyrim.h"
#include "selectiondialog.h"

#include "QDebug"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QApplication::installTranslator(&AssetsOptTranslator);

    SelectionDialog dialog(QObject::tr("Game mode"), nullptr);
    dialog.disableCancel();
    dialog.addChoice("Skyrim SE", QObject::tr("Skyrim SE"), QVariant());
    dialog.addChoice("Skyrim", QObject::tr("Skyrim"), QVariant());
    dialog.exec();

    QString choice = dialog.getChoiceString();

    QMainWindow *window;
    if(choice == "Skyrim SE")
        window = new SkyrimSE();
    else if(choice == "Skyrim")
        window = new Skyrim();
    else
        return 1;

    window->show();

    auto result = app.exec();
    delete window;
    return result;
}
