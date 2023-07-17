/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "gui/LevelSelector.hpp"
#include "gui/MainWindow.hpp"
#include "manager.hpp"
#include "settings/settings.hpp"
#include "version.hpp"

#include <plog/Log.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>

void init()
{
    QCoreApplication::setApplicationName("Cathedral Assets Optimizer");
    QCoreApplication::setApplicationVersion(CAO_VERSION);
}

void init_translations()
{
    // Qt translations
    static QTranslator qt_translator;
    if (qt_translator.load(QLocale::system(),
                           "qtbase",
                           "_",
                           QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(&qt_translator);
    }

    // CAO translations
    static QTranslator assets_opt_translator;
    if (qt_translator.load(QLocale::system(), "AssetsOpt", "_", "translations"))
    {
        QCoreApplication::installTranslator(&assets_opt_translator);
    }
}

void display_error(bool cli, const std::string &err)
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

int main(int argc, char *argv[])
{
    auto app = std::make_unique<QCoreApplication>(argc, argv);

    init();

    QCommandLineParser parser;
    parser.addPositionalArgument("profile", "The profile to use");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.process(*app);

    const bool cli = parser.isSet("cli");

    if (!cli)
    {
        app.reset(); // Destroying the QCoreApplication before creating a QApplication is required
        app = std::make_unique<QApplication>(argc, argv);
    }

    init_translations();

    try
    {
        auto settings = cao::load_settings();

        if (cli)
        {
            const auto profile_name = parser.positionalArguments().value(0);

            auto idx = settings.find_profile(btu::common::as_utf8_string(profile_name.toStdString()));
            if (!idx)
                throw std::runtime_error("Profile not found");

            cao::Manager manager(settings);
            manager.run_optimization();
        }
        else
        {
            auto window = cao::MainWindow{};

            cao::LevelSelector selector(settings);
            if (!selector.run_selection(window))
                return 0;

            window.show();
            selector.set_handler(window);

            return app->exec();
        }
    }
    catch (const std::exception &e)
    {
        display_error(cli, e.what());
        return 1;
    }

    return 0;
}
