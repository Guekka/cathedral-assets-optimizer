/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "settings/settings.hpp"

#include <doctest/doctest.h>

#include <cstddef>
#include <utility>

namespace cao {
TEST_CASE("legacy GUI modes migrate to Quick tasks and Customize")
{
    auto legacy_json = nlohmann::json(GuiSettings{});
    legacy_json.erase("workspace");
    legacy_json.erase("quick_target_game");
    legacy_json["remember_gui_mode"] = false;

    legacy_json["gui_mode"] = GuiMode::QuickOptimize;
    const auto quick_tasks  = legacy_json.get<GuiSettings>();
    CHECK(quick_tasks.workspace == Workspace::QuickTasks);

    auto quick_tasks_json = nlohmann::json(quick_tasks);
    CHECK(quick_tasks_json.at("workspace").get<Workspace>() == Workspace::QuickTasks);
    CHECK(quick_tasks_json.at("gui_mode").get<GuiMode>() == GuiMode::QuickOptimize);
    CHECK_FALSE(quick_tasks_json.at("remember_gui_mode").get<bool>());

    for (const auto legacy_mode : {GuiMode::Medium, GuiMode::Advanced})
    {
        legacy_json["gui_mode"] = legacy_mode;
        const auto customize    = legacy_json.get<GuiSettings>();
        CHECK(customize.workspace == Workspace::Customize);

        const auto customize_json = nlohmann::json(customize);
        CHECK(customize_json.at("workspace").get<Workspace>() == Workspace::Customize);
        CHECK(customize_json.at("gui_mode").get<GuiMode>() == GuiMode::Advanced);

        auto rollback_json = customize_json;
        rollback_json.erase("workspace");
        CHECK(rollback_json.get<GuiSettings>().workspace == Workspace::Customize);
    }

    auto workspace_wins         = quick_tasks_json;
    workspace_wins["workspace"] = Workspace::Customize;
    workspace_wins["gui_mode"]  = GuiMode::QuickOptimize;
    CHECK(workspace_wins.get<GuiSettings>().workspace == Workspace::Customize);
}

TEST_CASE("legacy settings migrate quick target game from the selected profile")
{
    auto settings = Settings::make_base();
    settings.create_profile(u8"Legacy SLE", Profile::make_base(btu::Game::SLE));
    REQUIRE(settings.set_current_profile(u8"Legacy SLE"));

    auto legacy_json = nlohmann::json(settings);
    legacy_json["gui"].erase("workspace");
    legacy_json["gui"].erase("quick_target_game");

    const auto migrated = legacy_json.get<Settings>();
    CHECK(migrated.gui.quick_target_game == btu::Game::SLE);
}

TEST_CASE("quick target game persists independently of the selected profile")
{
    auto settings                  = Settings::make_base();
    settings.gui.quick_target_game = btu::Game::SLE;

    const auto restored = nlohmann::json(settings).get<Settings>();
    CHECK(restored.gui.quick_target_game == btu::Game::SLE);
    CHECK(restored.current_profile().target_game == settings.current_profile().target_game);
}

TEST_CASE("legacy settings migrate run context from the selected profile")
{
    auto legacy_settings             = Settings::make_base();
    auto legacy_profile              = Profile::make_base(btu::Game::SSE);
    legacy_profile.input_path        = btu::Path{"guide-mods"};
    legacy_profile.optimization_mode = OptimizationMode::SeveralMods;
    legacy_profile.dry_run           = true;

    legacy_settings.create_profile(u8"Guide", legacy_profile);
    REQUIRE(legacy_settings.set_current_profile(u8"Guide"));
    legacy_settings.run_context = RunContext::from_profile(legacy_settings.current_profile());

    auto legacy_json = nlohmann::json(legacy_settings);
    legacy_json.erase("run_context");

    const auto migrated = legacy_json.get<Settings>();
    CHECK(migrated.run_context.input_path == legacy_profile.input_path);
    CHECK(migrated.run_context.optimization_mode == OptimizationMode::SeveralMods);
    CHECK(migrated.run_context.dry_run);
    CHECK(migrated.current_profile().input_path == legacy_profile.input_path);
}

TEST_CASE("settings serialize shared run context with legacy rollback fields")
{
    auto settings                    = Settings::make_base();
    auto legacy_profile              = Profile::make_base(btu::Game::FNV);
    legacy_profile.input_path        = btu::Path{"profile-specific-input"};
    legacy_profile.optimization_mode = OptimizationMode::SeveralMods;
    legacy_profile.dry_run           = true;

    settings.create_profile(u8"Guide", legacy_profile);
    REQUIRE(settings.set_current_profile(u8"Guide"));
    settings.run_context = {
        .input_path        = btu::Path{"shared-input"},
        .optimization_mode = OptimizationMode::SingleMod,
        .dry_run           = false,
    };

    const auto serialized = nlohmann::json(settings);
    REQUIRE(serialized.contains("run_context"));
    CHECK(settings.current_profile().input_path == legacy_profile.input_path);
    CHECK(settings.current_profile().optimization_mode == legacy_profile.optimization_mode);
    CHECK(settings.current_profile().dry_run == legacy_profile.dry_run);

    const auto restored = serialized.get<Settings>();
    CHECK(restored.run_context.input_path == settings.run_context.input_path);
    CHECK(restored.run_context.optimization_mode == settings.run_context.optimization_mode);
    CHECK(restored.run_context.dry_run == settings.run_context.dry_run);
    CHECK(restored.current_profile().input_path == settings.run_context.input_path);
    CHECK(restored.current_profile().optimization_mode == settings.run_context.optimization_mode);
    CHECK(restored.current_profile().dry_run == settings.run_context.dry_run);

    auto conflicting_json               = serialized;
    const auto active_index             = conflicting_json.at("current_profile_index_").get<std::size_t>();
    auto &active_legacy_profile         = conflicting_json.at("profiles_").at(active_index).at(1);
    active_legacy_profile["input_path"] = "stale-profile-input";
    active_legacy_profile["optimization_mode"] = nlohmann::json(OptimizationMode::SeveralMods);
    active_legacy_profile["dry_run"]           = true;
    const auto authoritative                   = conflicting_json.get<Settings>();
    CHECK(authoritative.run_context.input_path == settings.run_context.input_path);
    CHECK(authoritative.run_context.optimization_mode == settings.run_context.optimization_mode);
    CHECK(authoritative.run_context.dry_run == settings.run_context.dry_run);

    auto rollback_json = serialized;
    rollback_json.erase("run_context");
    const auto rollback_settings = rollback_json.get<Settings>();
    CHECK(rollback_settings.run_context.input_path == settings.run_context.input_path);
    CHECK(rollback_settings.run_context.optimization_mode == settings.run_context.optimization_mode);
    CHECK(rollback_settings.run_context.dry_run == settings.run_context.dry_run);

    const auto untouched_profile = restored.get_profile(u8"SSE");
    REQUIRE(untouched_profile.has_value());
    CHECK(untouched_profile->input_path.empty());
}

TEST_CASE("run settings project shared context without mutating saved profiles")
{
    auto settings                   = Settings::make_base();
    auto saved_profile              = Profile::make_base(btu::Game::FNV);
    saved_profile.input_path        = btu::Path{"saved-profile-input"};
    saved_profile.optimization_mode = OptimizationMode::SeveralMods;
    saved_profile.dry_run           = true;
    auto saved_rules                = saved_profile.per_file_settings();
    saved_rules.back()->pack        = false;

    settings.create_profile(u8"Guide", saved_profile);
    REQUIRE(settings.set_current_profile(u8"Guide"));
    settings.run_context = {
        .input_path        = btu::Path{"run-input"},
        .optimization_mode = OptimizationMode::SingleMod,
        .dry_run           = false,
    };

    auto quick_profile      = Profile::make_base(btu::Game::SSE);
    const auto run_settings = make_run_settings(settings, std::optional<Profile>{std::move(quick_profile)});

    CHECK(run_settings.current_profile().target_game == btu::Game::SSE);
    CHECK(run_settings.current_profile().input_path == settings.run_context.input_path);
    CHECK(run_settings.current_profile().optimization_mode == settings.run_context.optimization_mode);
    CHECK(run_settings.current_profile().dry_run == settings.run_context.dry_run);
    CHECK(run_settings.current_profile().per_file_settings().back()->pack);

    CHECK(settings.current_profile().target_game == btu::Game::FNV);
    CHECK(settings.current_profile().input_path == saved_profile.input_path);
    CHECK(settings.current_profile().optimization_mode == saved_profile.optimization_mode);
    CHECK(settings.current_profile().dry_run == saved_profile.dry_run);
    CHECK_FALSE(settings.current_profile().per_file_settings().back()->pack);
}
} // namespace cao
