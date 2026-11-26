/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "gui/MainWindow.hpp"
#include "gui/utils/utils.hpp"
#include "logger.hpp"
#include "manager.hpp"
#include "settings/json.hpp"
#include "settings/settings.hpp"
#include "version.hpp"

#include <plog/Log.h>

#include <QApplication>
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTranslator>

#include <filesystem>
#include <utility>

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
    parser.addPositionalArgument("profile", "The profile name, or a JSON path when using --json");
    parser.addPositionalArgument(
        "modPaths", "Optional mod paths to process instead of the profile's input path", "[modPaths...]");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.addOption({"json", "Load the profile from a JSON file"});
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
            const auto positional_args = parser.positionalArguments();
            const auto profile_name    = positional_args.value(0);
            const auto load_from_json  = parser.isSet("json");

            if (profile_name.isEmpty())
            {
                const auto message
                    = load_from_json ? "Profile JSON path is required when using --json"
                                     : "Profile name is required when using --cli";
                throw std::runtime_error(message);
            }

            auto cli_settings = settings;
            if (load_from_json)
            {
                const auto json_path = std::filesystem::path(cao::to_u8string(profile_name));
                auto profile = cao::json::read_from_file<cao::Profile>(json_path);
                if (!profile)
                {
                    throw std::runtime_error("Failed to load profile from JSON file: " + json_path.string()
                                             + ". Check that the file exists and has a valid profile format.");
                }

                cli_settings.current_profile() = std::move(*profile);
            }
            else if (!cli_settings.set_current_profile(cao::to_u8string(profile_name)))
            {
                throw std::runtime_error("Profile not found: " + profile_name.toStdString());
            }

            if (positional_args.size() == 1)
            {
                cao::Manager manager;
                manager.run_optimization(std::move(cli_settings), std::stop_token{}); // TODO: handle signals
            }
            else
            {
                for (qsizetype i = 1; i < positional_args.size(); ++i)
                {
                    const auto &mod_path = positional_args.at(i);
                    if (mod_path.isEmpty())
                        throw std::runtime_error("Mod paths cannot be empty");

                    auto mod_settings = cli_settings;
                    auto &profile = mod_settings.current_profile();
                    profile.input_path = std::filesystem::path(cao::to_u8string(mod_path));
                    profile.optimization_mode = cao::OptimizationMode::SingleMod;

                    cao::Manager manager;
                    manager.run_optimization(std::move(mod_settings), std::stop_token{});
                }
            }
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
