/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "GUI/LevelSelector.hpp"
#include "GUI/MainWindow.hpp"
#include "GUI/Utils/SetTheme.hpp"
#include "Version.hpp"
#include "manager.hpp"
#include "settings/settings.hpp"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>

void init()
{
    Q_INIT_RESOURCE(style);

    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(CAO_VERSION);
}

void initTranslations()
{
    static QTranslator qtTranslator;
    qtTranslator.load(QLocale(), "qt", "_", "translations");
    QCoreApplication::installTranslator(&qtTranslator);

    static QTranslator AssetsOptTranslator;
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
    // PLOG_FATAL << err;
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
    // PLOG_INFO << text;
}

void migrateProfiles(bool cli)
{
    /*
    const auto &migratedProfiles = cao::migrateProfiles(QDir("importedProfiles"), QDir("profiles")); FIXME
    if (migratedProfiles.empty())
        return;

    const QString &text = QCoreApplication::translate("main",
                                                      "Migrated profiles:\n%1. Please check that they were "
                                                      "assigned the right game. This process in not perfect")
                              .arg(migratedProfiles.join('\n'));

    cao::getProfiles().update(true);

    displayInfo(cli, text.toStdString());
     FIXME
        */
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

    initTranslations();

    try
    {
        migrateProfiles(cli);

        if (cli)
        {
            auto settings = cao::load_settings();

            const auto profile_name = parser.positionalArguments().value(0);

            auto idx = settings.find_profile(btu::common::as_utf8_string(profile_name.toStdString()));
            if (!idx)
                throw std::runtime_error("Profile not found");

            cao::Manager manager(settings);
            manager.run_optimization();
        }
        else
        {
            auto window   = std::make_unique<cao::MainWindow>();
            auto settings = cao::load_settings();

            cao::LevelSelector selector(settings);
            if (!selector.run_selection(*window))
                return 0;

            window->show();
            selector.set_handler(*window);

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
