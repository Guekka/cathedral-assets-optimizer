/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "gui/MainWindow.hpp"
#include "gui/utils/utils.hpp"
#include "logger.hpp"
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
    QCoreApplication::setApplicationVersion(k_cao_version);
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
    std::cerr << err << '\n' << std::flush;
    PLOG_FATAL << err;
    if (!cli)
        QMessageBox::critical(nullptr, "Unknown error", QString::fromStdString(err));
}

auto main(int argc, char *argv[]) -> int
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
        if (!cao::init_logging(cao::Settings::state_directory()))
            throw std::runtime_error("Failed to initialize logging.");

        auto settings = cao::load_settings();

        if (cli)
        {
            const auto profile_name = parser.positionalArguments().value(0);

            auto profile = settings.get_profile(cao::to_u8string(profile_name));
            if (!profile)
                throw std::runtime_error("Profile not found");

            cao::Manager manager;
            manager.run_optimization(settings, std::stop_token{}); // TODO: handle signals
        }
        else
        {
            auto window = cao::MainWindow{settings};
            window.show();
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
