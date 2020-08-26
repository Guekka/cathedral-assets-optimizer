/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GUI/LevelSelector.hpp"
#include "GUI/MainWindow.hpp"
#include "GUI/Utils/SetTheme.hpp"
#include "Manager.hpp"
#include "Settings/MigrateProfiles.hpp"
#include "Version.hpp"

void init()
{
    Q_INIT_RESOURCE(style);

    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(CAO_VERSION);

    QTranslator qtTranslator;
    qtTranslator.load(QLocale(), "qt", "_", "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    QTranslator AssetsOptTranslator;
    AssetsOptTranslator.load(QLocale(), "AssetsOpt", "_", "translations");
    QCoreApplication::installTranslator(&AssetsOptTranslator);
}

void displayError(bool cli, const std::string &err)
{
    if (cli)
    {
        std::cerr << err << std::endl;
    }
    else
    {
        QMessageBox box(QMessageBox::Critical, "Unknown error", QString::fromStdString(err));
        box.exec();
    }
    PLOG_FATAL << err;
}

void displayInfo(bool cli, const std::string &text)
{
    if (cli)
    {
        std::cout << text << std::endl;
    }
    else
    {
        QMessageBox box(QMessageBox::Information, "Information", QString::fromStdString(text));
        box.exec();
    }
    PLOG_INFO << text;
}

void migrateProfiles(bool cli)
{
    const auto &migratedProfiles = CAO::migrateProfiles(QDir("importedProfiles"), QDir("profiles"));
    if (migratedProfiles.empty())
        return;

    const QString &text = QCoreApplication::translate("main",
                                                      "Migrated profiles:\n%1. Please check that they were "
                                                      "assigned the right game. This process in not perfect")
                              .arg(migratedProfiles.join('\n'));

    CAO::getProfiles().update(true);

    displayInfo(cli, text.toStdString());
}

int main(int argc, char *argv[])
{
    std::unique_ptr<QCoreApplication> app = std::make_unique<QCoreApplication>(argc, argv);

    init();

    QCommandLineParser parser;
    parser.addPositionalArgument("profile", "The profile to use");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.process(*app);

    const bool cli = parser.isSet("cli");

    if (!cli)
    {
        app = nullptr;
        app = std::make_unique<QApplication>(argc, argv);
    }

    try
    {
        migrateProfiles(cli);

        if (cli)
        {
            CAO::getProfiles().setCurrent(parser.positionalArguments().at(0));

            CAO::Manager manager;
            manager.runOptimization();
        }
        else
        {
            app    = nullptr;
            app    = std::make_unique<QApplication>(argc, argv);
            auto window = std::make_unique<CAO::MainWindow>();

            CAO::LevelSelector selector;
            if (!selector.runSelection(*window))
                return 0;

            window->show();
            selector.setHandler(*window);

            return app->exec();
        }
    }
    catch (const std::exception &e)
    {
        displayError(cli, e.what());
        return 1;
    }

    return 0;
}
