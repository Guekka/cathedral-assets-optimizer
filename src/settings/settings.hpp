/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "profile.hpp"

namespace cao {
struct GuiSettings
{
    GuiTheme gui_theme     = GuiTheme::Dark;
    bool remember_gui_mode = false;
    GuiMode gui_mode       = GuiMode::QuickOptimize;

    bool first_run     = true;
    bool show_tutorial = true;

    int gpu_index             = 0;
    bool delete_empty_folders = false; // TODO: implement
};

class Settings
{
public:
    void create_profile(const std::u8string &profile_name);
    void create_profile_from_base(const std::u8string &profile_name, const std::u8string &base_profile_name);

    [[nodiscard]] auto current_profile() const noexcept -> Profile;
    [[nodiscard]] auto set_current_profile(size_t index) noexcept -> bool;

    [[nodiscard]] auto find_profile(std::u8string_view profile_name) const noexcept -> std::optional<size_t>;

    void remove(size_t index) noexcept;

    static constexpr auto k_app_name = "cathedral_assets_optimizer";

    [[nodiscard]] static auto data_directory() noexcept -> std::filesystem::path;
    [[nodiscard]] static auto config_directory() noexcept -> std::filesystem::path;

    GuiSettings gui{};

private:
    std::vector<Profile> profiles_;
    size_t current_profile_index_ = 0;
};

void to_json(nlohmann::json &j, const Settings &settings);
void from_json(const nlohmann::json &j, Settings &settings);

[[nodiscard]] auto save_settings(const Settings &settings) -> bool;
[[nodiscard]] auto load_settings() -> Settings;

} // namespace cao
