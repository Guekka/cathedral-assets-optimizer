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

static void display_error(bool cli, const std::string &err)
{
    std::cerr << err << '\n' << std::flush;
    PLOG_FATAL << err;
    if (!cli)
        QMessageBox::critical(nullptr, "Unknown error", QString::fromStdString(err));
}

[[nodiscard]] auto parse_mod_paths(const QStringList &positional_args) -> std::vector<std::filesystem::path>
{
    std::vector<std::filesystem::path> mod_paths;
    // Skip first argument (profile name/path), rest are mod paths
    for (int i = 1; i < positional_args.size(); ++i)
        mod_paths.emplace_back(cao::to_u8string(positional_args[i]));

    return mod_paths;
}

void run_optimization_for_path(cao::Settings &settings,
                               cao::Profile profile,
                               const std::filesystem::path &path)
{
    profile.input_path         = path;
    settings.current_profile() = profile;

    // Manager stores state, so create a new instance for each run
    cao::Manager manager;
    manager.run_optimization(settings, std::stop_token{}); // TODO: handle signals
}

void run_cli_mode(cao::Settings &settings, const QCommandLineParser &parser)
{
    const auto positional_args      = parser.positionalArguments();
    const auto profile_name_or_path = positional_args.value(0);
    const bool load_from_json       = parser.isSet("json");

    if (profile_name_or_path.isEmpty())
    {
        const std::string error_msg
            = load_from_json ? "Profile JSON path is required when using CLI mode with -json flag"
                             : "Profile name is required when using CLI mode";
        throw std::runtime_error(error_msg);
    }

    const auto profile = [&]() -> cao::Profile {
        if (load_from_json)
        {
            const auto json_path = std::filesystem::path(cao::to_u8string(profile_name_or_path));
            auto loaded_profile  = cao::json::read_from_file<cao::Profile>(json_path);
            if (!loaded_profile)
                throw std::runtime_error("Failed to load profile from JSON file: " + json_path.string()
                                         + ". Check file exists and format is valid.");
            return *std::move(loaded_profile);
        }

        auto loaded_profile = settings.get_profile(cao::to_u8string(profile_name_or_path));
        if (!loaded_profile)
            throw std::runtime_error("Profile not found: " + profile_name_or_path.toStdString());
        return *std::move(loaded_profile);
    }();

    auto mod_paths = parse_mod_paths(positional_args);

    // If mod paths are provided on command line, process each one
    if (!mod_paths.empty())
    {
        for (const auto &mod_path : mod_paths)
        {
            run_optimization_for_path(settings, profile, mod_path);
        }
    }
    else
    {
        // No additional mod paths, use the profile's input_path
        run_optimization_for_path(settings, profile, profile.input_path);
    }
}

auto main(int argc, char *argv[]) -> int
{
    auto app = std::make_unique<QCoreApplication>(argc, argv);

    init();

    QCommandLineParser parser;
    parser.addPositionalArgument("profile", "The profile to use");
    parser.addPositionalArgument("paths", "Additional mod paths to process (optional)", "[paths...]");
    parser.addOption({"cli", "Do not run the GUI"});
    parser.addOption({"json", "Load profile from JSON file instead of using saved profiles"});
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
            run_cli_mode(settings, parser);
        }
        else
        {
            auto window = cao::MainWindow{settings};
            window.show();
            return QCoreApplication::exec();
        }
    }
    catch (const std::exception &e)
    {
        display_error(cli, e.what());
        return 1;
    }

    return 0;
}
