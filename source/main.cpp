/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Version.hpp"
#ifdef GUI
#include "GUI/MainWindow.hpp"
#endif
#include "Manager.hpp"

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
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);

#ifdef GUI
    auto window = std::make_unique<CAO::MainWindow>();
#else
    Manager *manager = new Manager(QCoreApplication::arguments());
#endif

    try
    {
#ifdef GUI
        window->show();
#else
        manager->runOptimization();
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        PLOG_FATAL << e.what();
        return 1;
    }

#ifdef GUI
    return QApplication::exec();
#else
    return 0;
#endif
}
