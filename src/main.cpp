/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GUI/LevelSelector.hpp"
#include "GUI/MainWindow.hpp"
#include "Manager.hpp"
#include "Version.hpp"

void init()
{
    Q_INIT_RESOURCE(style);

    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(CAO_VERSION);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load("AssetsOpt_" + QLocale::system().name(), "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);
}

int main(int argc, char *argv[])
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);

    init();

    QCommandLineParser parser;
    parser.addPositionalArgument("profile", "The profile to use");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.process(*app);

    std::unique_ptr<CAO::MainWindow> window;
    std::unique_ptr<CAO::Manager> manager;

    try
    {
        CAO::Profiles::getInstance().update(true);
        if (parser.isSet("cli"))
        {
            CAO::Profiles::getInstance().setCurrent(parser.positionalArguments().at(0));

            manager = std::make_unique<CAO::Manager>(CAO::currentProfile());
            manager->runOptimization();
        }
        else
        {
            app    = nullptr;
            app    = std::make_unique<QApplication>(argc, argv);
            window = std::make_unique<CAO::MainWindow>();
            CAO::LevelSelector selector(*window);
            selector.exec();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        PLOG_FATAL << e.what();
        return 1;
    }

    return app->exec();
}
