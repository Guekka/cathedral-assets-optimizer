/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "profile.hpp"

#include <nlohmann/json.hpp>

namespace cao {
struct GuiSettings
{
    GuiTheme gui_theme     = GuiTheme::Dark;
    bool remember_gui_mode = false;
    GuiMode gui_mode       = GuiMode::QuickOptimize;

    bool first_run = true;

    std::u8string selected_pattern = k_default_pattern.text();

    size_t gpu_index = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    GuiSettings, gui_theme, remember_gui_mode, gui_mode, first_run, selected_pattern, gpu_index)

class Settings
{
public:
    void create_profile(std::u8string profile_name, Profile base_profile) noexcept;

    [[nodiscard]] auto current_profile() const noexcept -> const Profile &;
    [[nodiscard]] auto current_profile() noexcept -> Profile &;

    [[nodiscard]] auto current_profile_name() const noexcept -> std::u8string_view;

    [[nodiscard]] auto set_current_profile(std::u8string_view profile) noexcept -> bool;
    [[nodiscard]] auto get_profile(std::u8string_view profile_name) const noexcept -> std::optional<Profile>;

    [[nodiscard]] auto list_profiles() const noexcept -> std::vector<std::u8string_view>;

    void remove(std::u8string_view profile) noexcept;

    [[nodiscard]] static auto make_base() noexcept -> Settings;

    static constexpr auto k_app_name = "cathedral_assets_optimizer";

    [[nodiscard]] static auto config_directory() noexcept -> std::filesystem::path;
    [[nodiscard]] static auto state_directory() noexcept -> std::filesystem::path;

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    GuiSettings gui{};

private:
    std::vector<std::pair<std::u8string, Profile>> profiles_;
    size_t current_profile_index_ = 0;

public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Settings, gui, profiles_, current_profile_index_)
};

[[nodiscard]] auto current_per_file_settings(Settings &sets) noexcept -> PerFileSettings &;
[[nodiscard]] auto current_per_file_settings(const Settings &sets) noexcept -> const PerFileSettings &;

[[nodiscard]] auto save_settings(const Settings &settings) -> bool;

/// @brief Loads settings from the default location.
/// @return The loaded settings or a default-constructed Settings object if the file could not be
/// loaded.
[[nodiscard]] auto load_settings() -> Settings;

} // namespace cao
