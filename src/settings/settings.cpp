/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include "json.hpp"

#include <flux.hpp>
#include <sago/platform_folders.h>

#include <stdexcept>
#include <utility>

namespace cao {

auto RunContext::from_profile(const Profile &profile) -> RunContext
{
    return {profile.input_path, profile.optimization_mode, profile.dry_run};
}

void RunContext::apply_to(Profile &profile) const
{
    profile.input_path        = input_path;
    profile.optimization_mode = optimization_mode;
    profile.dry_run           = dry_run;
}

void to_json(nlohmann::json &json, const GuiSettings &settings)
{
    const auto legacy_mode = settings.workspace == Workspace::QuickTasks ? GuiMode::QuickOptimize
                                                                         : GuiMode::Advanced;

    json = {
        {"gui_theme", settings.gui_theme},
        {"remember_gui_mode", settings.remember_gui_mode},
        {"gui_mode", legacy_mode},
        {"first_run", settings.first_run},
        {"selected_pattern", settings.selected_pattern},
        {"workspace", settings.workspace},
        {"quick_target_game", settings.quick_target_game},
    };
}

void from_json(const nlohmann::json &json, GuiSettings &settings)
{
    json.at("gui_theme").get_to(settings.gui_theme);
    if (const auto remember = json.find("remember_gui_mode"); remember != json.end())
        remember->get_to(settings.remember_gui_mode);
    json.at("first_run").get_to(settings.first_run);
    json.at("selected_pattern").get_to(settings.selected_pattern);

    if (const auto workspace = json.find("workspace"); workspace != json.end())
    {
        workspace->get_to(settings.workspace);
    }
    else
    {
        auto legacy_mode = GuiMode::QuickOptimize;
        json.at("gui_mode").get_to(legacy_mode);
        settings.workspace = legacy_mode == GuiMode::QuickOptimize ? Workspace::QuickTasks
                                                                   : Workspace::Customize;
    }
    if (const auto quick_target_game = json.find("quick_target_game"); quick_target_game != json.end())
        quick_target_game->get_to(settings.quick_target_game);
}

void to_json(nlohmann::json &json, const Settings &settings)
{
    auto profiles = settings.profiles_;
    // Keep this compatibility mirror until rollback to versions without run_context is no longer supported.
    if (!profiles.empty())
        settings.run_context.apply_to(profiles.at(settings.current_profile_index_).second);

    json = {
        {"gui", settings.gui},
        {"profiles_", std::move(profiles)},
        {"current_profile_index_", settings.current_profile_index_},
        {"run_context", settings.run_context},
    };
}

void from_json(const nlohmann::json &json, Settings &settings)
{
    json.at("profiles_").get_to(settings.profiles_);
    json.at("current_profile_index_").get_to(settings.current_profile_index_);
    const auto &gui_json             = json.at("gui");
    const bool has_quick_target_game = gui_json.contains("quick_target_game");
    gui_json.get_to(settings.gui);
    if (!has_quick_target_game)
    {
        const auto legacy_target_game = settings.current_profile().target_game;
        if (legacy_target_game == btu::Game::SLE || legacy_target_game == btu::Game::SSE)
            settings.gui.quick_target_game = legacy_target_game;
    }

    if (const auto run_context = json.find("run_context"); run_context != json.end())
        run_context->get_to(settings.run_context);
    else
        // Older settings files stored run context on the selected profile.
        settings.run_context = RunContext::from_profile(settings.current_profile());
}

void Settings::create_profile(std::u8string profile_name, Profile base_profile) noexcept
{
    if (get_profile(profile_name))
        return;

    profiles_.emplace_back(std::move(profile_name), std::move(base_profile));
}

auto Settings::current_profile() const noexcept -> const Profile &
{
    return profiles_.at(current_profile_index_).second;
}

auto Settings::current_profile() noexcept -> Profile &
{
    return profiles_.at(current_profile_index_).second;
}

auto Settings::current_profile_name() const noexcept -> std::u8string_view
{
    return profiles_.at(current_profile_index_).first;
}

void Settings::remove(std::u8string_view profile) noexcept
{
    const auto it = std::ranges::find_if(profiles_, [profile](const auto &p) { return p.first == profile; });
    if (it == profiles_.end())
        return;

    if (const auto current_profile = current_profile_name(); current_profile == profile)
        current_profile_index_ = 0;

    profiles_.erase(it);

    if (current_profile_index_ >= profiles_.size())
        current_profile_index_ = profiles_.size() - 1;
}

auto Settings::set_current_profile(std::u8string_view profile) noexcept -> bool
{
    const auto it = std::ranges::find_if(profiles_, [profile](const auto &p) { return p.first == profile; });
    if (it == profiles_.end())
        return false;

    current_profile_index_ = std::distance(profiles_.begin(), it);
    return true;
}

auto Settings::get_profile(std::u8string_view profile_name) const noexcept -> std::optional<Profile>
{
    const auto it = std::ranges::find_if(profiles_,
                                         [profile_name](const auto &p) { return p.first == profile_name; });
    if (it == profiles_.end())
        return std::nullopt;

    return it->second;
}

auto Settings::make_base() noexcept -> Settings
{
    Settings settings;
    settings.profiles_ = {
        {u8"FNV", Profile::make_base(btu::Game::FNV)},
        {u8"SSE", Profile::make_base(btu::Game::SSE)},
        {u8"SLE", Profile::make_base(btu::Game::SLE)},
        {u8"FO4NG", Profile::make_base(btu::Game::FO4)},
        {u8"Starfield", Profile::make_base(btu::Game::Starfield)},
    };
    settings.current_profile_index_ = 0;
    settings.run_context            = RunContext::from_profile(settings.current_profile());

    return settings;
}

using SagoFunction = std::string (*)();

[[nodiscard]] auto any_directory(SagoFunction func,
                                 const std::filesystem::path &default_path) noexcept -> std::filesystem::path
{
    try
    {
        const auto base = std::filesystem::path(func());

        auto path = base / Settings::k_app_name;
        std::filesystem::create_directories(path);

        return path;
    }
    catch (const std::exception &)
    {
        return std::filesystem::absolute(default_path);
    }
}

auto Settings::config_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getConfigHome, "config");
}

auto Settings::state_directory() noexcept -> std::filesystem::path
{
    return any_directory(&sago::getStateDir, "state");
}

auto Settings::list_profiles() const noexcept -> std::vector<std::u8string_view>
{
    return flux::ref(profiles_)
        .map([](const auto &pair) -> std::u8string_view { return pair.first; })
        .to<std::vector>();
}

auto load_settings() -> Settings
{
    const auto settings_file = Settings::config_directory() / "settings.json";
    if (!std::filesystem::exists(settings_file))
        return Settings::make_base();

    auto settings = json::read_from_file<Settings>(settings_file);
    if (!settings)
        throw std::runtime_error("Could not load settings file; it has been left unchanged: "
                                 + settings_file.string());

    return std::move(*settings);
}

auto save_settings(const Settings &settings) -> bool
{
    const auto settings_file = Settings::config_directory() / "settings.json";
    return json::save_to_file(settings, settings_file);
}

auto make_run_settings(const Settings &settings, std::optional<Profile> profile_override) -> Settings
{
    auto run_settings = settings;
    if (profile_override)
        run_settings.current_profile() = std::move(*profile_override);

    run_settings.run_context.apply_to(run_settings.current_profile());
    return run_settings;
}

auto current_per_file_settings(Settings &sets) -> PerFileSettings &
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return const_cast<PerFileSettings &>(current_per_file_settings(std::as_const(sets)));
}

auto current_per_file_settings(const Settings &sets) -> const PerFileSettings &
{
    const auto &profile         = sets.current_profile();
    const auto selected_pattern = sets.gui.workspace == Workspace::Customize ? sets.gui.selected_pattern
                                                                             : k_default_pattern.text();

    auto per_file_settings = profile.per_file_settings();

    const auto it = std::ranges::find_if(per_file_settings, [selected_pattern](const auto *pfs) {
        return pfs->pattern.text() == selected_pattern;
    });

    if (it == per_file_settings.end())
        throw std::logic_error("This should never happen");

    return **it;
}

} // namespace cao
