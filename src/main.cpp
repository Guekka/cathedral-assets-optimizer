/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Version.h"
#ifdef GUI
#include "MainWindow.h"
#endif
#include "Manager.h"

void displayError(const std::string &err)
{
#ifdef GUI
    QMessageBox box(QMessageBox::Critical, "Unknown error", QString::fromStdString(err));
    box.exec();
#else
    std::cerr << err << std::endl;
#endif

    PLOG_FATAL << err;
}

int main(int argc, char *argv[])
{
#ifdef GUI
    QApplication app(argc, argv);
#else
    QCoreApplication app(argc, argv);
#endif

    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(CAO_VERSION);

    QTranslator qtTranslator;
    qtTranslator.load(QLocale(), "qt", "_", "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    qtTranslator.load(QLocale(), "AssetsOpt", "_", "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);

#ifdef GUI
    MainWindow *window = new MainWindow;
#else
    Manager *manager = new Manager(QCoreApplication::arguments());
#endif

    /*try
    {*/
#ifdef GUI
        window->show();
#else
        manager->runOptimization();
#endif
        /*}
        catch (const std::exception &e)
        {
            displayError(e.what());
            return 1;
        }*/

#ifdef GUI
    return QApplication::exec();
#else
    return 0;
#endif
}
