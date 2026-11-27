/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "gui/file_rule_preview.hpp"
#include "settings/per_file_settings.hpp"
#include "settings/profile.hpp"
#include "settings/settings.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

namespace cao {
namespace {
class TemporaryDirectory
{
public:
    TemporaryDirectory()
    {
        const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        path_             = std::filesystem::temp_directory_path() / ("cao-file-rule-preview-" + suffix);
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory()
    {
        auto error = std::error_code{};
        std::filesystem::remove_all(path_, error);
    }

    [[nodiscard]] auto path() const noexcept -> const std::filesystem::path & { return path_; }

private:
    std::filesystem::path path_;
};
} // namespace

TEST_CASE("simple file rules match folders and extensions")
{
    const auto pattern = Pattern::simple_rule(u8"textures/interface/", u8"dds");

    CHECK(pattern.matches(std::filesystem::path{u8"textures/interface/menu.dds"}));
    CHECK(pattern.matches(std::filesystem::path{u8"mods/Textures/Interface/sub/menu.DDS"}));
    CHECK_FALSE(pattern.matches(std::filesystem::path{u8"textures/interface-old/menu.dds"}));
    CHECK_FALSE(pattern.matches(std::filesystem::path{u8"textures/interface/menu.nif"}));
}

TEST_CASE("simple file rules can match by folder or extension alone")
{
    const auto folder_rule    = Pattern::simple_rule(u8"textures/interface", {});
    const auto extension_rule = Pattern::simple_rule({}, u8"dds");

    CHECK(folder_rule.matches(std::filesystem::path{u8"textures/interface/menu.nif"}));
    CHECK_FALSE(folder_rule.matches(std::filesystem::path{u8"textures/landscape/menu.nif"}));
    CHECK(extension_rule.matches(std::filesystem::path{u8"textures/landscape/menu.DDS"}));
    CHECK_FALSE(extension_rule.matches(std::filesystem::path{u8"textures/landscape/menu.nif"}));
}

TEST_CASE("simple file rules reject paths outside the input root")
{
    CHECK_THROWS_AS(static_cast<void>(Pattern::simple_rule(u8"../textures", u8"dds")), std::invalid_argument);
    CHECK_THROWS_AS(static_cast<void>(Pattern::simple_rule(u8"/textures", u8"dds")), std::invalid_argument);
    CHECK_THROWS_AS(static_cast<void>(Pattern::simple_rule(u8"textures/*", u8"dds")), std::invalid_argument);
}

TEST_CASE("simple file rules serialize their conditions and name")
{
    const auto pattern = Pattern::simple_rule(u8"textures/interface", u8"dds", u8"Interface textures");
    auto loaded        = Pattern::simple_rule(u8"textures/landscape", u8"nif");
    nlohmann::json(pattern).get_to(loaded);

    REQUIRE(loaded.simple_rule() != nullptr);
    CHECK(loaded.simple_rule()->folder == u8"textures/interface");
    CHECK(loaded.simple_rule()->extension == u8".dds");
    CHECK(loaded.name() == std::u8string_view{u8"Interface textures"});
    CHECK(loaded.text() == pattern.text());
    CHECK(loaded.matches(std::filesystem::path{u8"textures/interface/menu.DDS"}));
}

TEST_CASE("legacy glob and regex patterns still deserialize")
{
    auto wildcard = Pattern::wildcard(u8"*.nif");
    auto regex    = Pattern::regular_expression(u8".*\\.nif");
    nlohmann::json{{"pattern", "*.dds"}, {"type", 0}}.get_to(wildcard);
    nlohmann::json{{"pattern", R"(.*\.dds)"}, {"type", 1}}.get_to(regex);

    CHECK(wildcard.matches(std::filesystem::path{u8"menu.dds"}));
    CHECK(regex.matches(std::filesystem::path{u8"textures/menu.DDS"}));
}

TEST_CASE("profiles apply the first matching simple file rule")
{
    auto profile = Profile::make_base(btu::Game::SSE);

    auto all_dds          = PerFileSettings::make_base(btu::Game::SSE);
    all_dds.pattern       = Pattern::simple_rule({}, u8"dds", u8"All DDS textures");
    auto interface_dds    = PerFileSettings::make_base(btu::Game::SSE);
    interface_dds.pattern = Pattern::simple_rule(u8"textures/interface", u8"dds", u8"Interface textures");

    profile.prepend_per_file_settings(std::move(all_dds));
    profile.prepend_per_file_settings(std::move(interface_dds));

    const auto selected = profile.get_per_file_settings(
        std::filesystem::path{u8"textures/interface/menu.dds"});
    CHECK(selected.pattern.name() == std::u8string_view{u8"Interface textures"});

    const auto fallback = profile.get_per_file_settings(std::filesystem::path{u8"meshes/armor.nif"});
    CHECK(fallback.pattern.text() == k_default_pattern.text());
}

TEST_CASE("moving a file rule changes first-match priority")
{
    auto profile = Profile::make_base(btu::Game::SSE);

    auto all_dds          = PerFileSettings::make_base(btu::Game::SSE);
    all_dds.pattern       = Pattern::simple_rule({}, u8"dds", u8"All DDS textures");
    auto interface_dds    = PerFileSettings::make_base(btu::Game::SSE);
    interface_dds.pattern = Pattern::simple_rule(u8"textures/interface", u8"dds", u8"Interface textures");

    profile.append_per_file_settings(std::move(all_dds));
    profile.append_per_file_settings(std::move(interface_dds));

    const auto interface_path = std::filesystem::path{u8"textures/interface/menu.dds"};
    CHECK(profile.get_per_file_settings(interface_path).pattern.name() == u8"All DDS textures");

    profile.move_per_file_settings(1, 0);

    CHECK(profile.get_per_file_settings(interface_path).pattern.name() == u8"Interface textures");
    CHECK(
        profile.get_per_file_settings(std::filesystem::path{u8"textures/landscape/ground.dds"}).pattern.name()
        == u8"All DDS textures");
}

TEST_CASE("file-rule impact previews first-match counts and fallback examples")
{
    auto profile = Profile::make_base(btu::Game::SSE);

    auto all_dds          = PerFileSettings::make_base(btu::Game::SSE);
    all_dds.pattern       = Pattern::simple_rule({}, u8"dds", u8"All DDS textures");
    auto interface_dds    = PerFileSettings::make_base(btu::Game::SSE);
    interface_dds.pattern = Pattern::simple_rule(u8"textures/interface", u8"dds", u8"Interface textures");

    profile.prepend_per_file_settings(std::move(all_dds));
    profile.prepend_per_file_settings(std::move(interface_dds));

    const auto files = std::vector<std::filesystem::path>{
        u8"textures/interface/menu.dds",
        u8"textures/landscape/ground.dds",
        u8"meshes/armor.nif",
        u8"textures/interface/icon.dds",
    };
    const auto impact = analyze_file_rule_impact(profile, files, 1);

    REQUIRE(impact.size() == 3);
    CHECK(impact[0].matching_files == 2);
    CHECK(impact[0].applied_files == 2);
    REQUIRE(impact[0].example_paths.size() == 1);
    CHECK(impact[0].example_paths[0] == std::filesystem::path{u8"textures/interface/icon.dds"});

    CHECK(impact[1].matching_files == 3);
    CHECK(impact[1].applied_files == 1);
    REQUIRE(impact[1].example_paths.size() == 1);
    CHECK(impact[1].example_paths[0] == std::filesystem::path{u8"textures/landscape/ground.dds"});
    REQUIRE(impact[1].overridden_example_paths.size() == 1);
    CHECK(impact[1].overridden_example_paths[0] == std::filesystem::path{u8"textures/interface/icon.dds"});

    CHECK(impact[2].applied_files == 1);
    REQUIRE(impact[2].example_paths.size() == 1);
    CHECK(impact[2].example_paths[0] == std::filesystem::path{u8"meshes/armor.nif"});
}

TEST_CASE("file-rule impact reports fully shadowed rules")
{
    auto profile = Profile::make_base(btu::Game::SSE);

    auto all_dds          = PerFileSettings::make_base(btu::Game::SSE);
    all_dds.pattern       = Pattern::simple_rule({}, u8"dds", u8"All DDS textures");
    auto interface_dds    = PerFileSettings::make_base(btu::Game::SSE);
    interface_dds.pattern = Pattern::simple_rule(u8"textures/interface", u8"dds", u8"Interface textures");

    profile.append_per_file_settings(std::move(all_dds));
    profile.append_per_file_settings(std::move(interface_dds));

    const auto files = std::vector<std::filesystem::path>{
        u8"textures/interface/menu.dds",
        u8"textures/landscape/ground.dds",
    };
    const auto impact = analyze_file_rule_impact(profile, files, 1);

    REQUIRE(impact.size() == 3);
    CHECK(impact[0].applied_files == 2);
    CHECK(impact[1].matching_files == 1);
    CHECK(impact[1].applied_files == 0);
    CHECK(impact[1].example_paths.empty());
    REQUIRE(impact[1].overridden_example_paths.size() == 1);
    CHECK(impact[1].overridden_example_paths[0] == files[0]);
}

TEST_CASE("file-rule preview scans single and several mods")
{
    auto temporary_directory = TemporaryDirectory{};
    const auto root          = temporary_directory.path();
    const auto create_file   = [](const std::filesystem::path &path) {
        std::filesystem::create_directories(path.parent_path());
        auto file = std::ofstream{path};
        file << "fixture";
        CHECK(file.good());
    };

    create_file(root / "root.dds");
    create_file(root / "mod-a" / "textures" / "interface.dds");
    create_file(root / "mod-b" / "meshes" / "armor.nif");

    const auto single_mod = scan_file_rule_preview(root, OptimizationMode::SingleMod, btu::Game::SSE);
    REQUIRE(single_mod.has_value());
    CHECK(single_mod->skipped_archives == 0);
    const auto expected_single = std::vector<std::filesystem::path>{
        "mod-a/textures/interface.dds",
        "mod-b/meshes/armor.nif",
        "root.dds",
    };
    auto single_files = single_mod->files;
    std::ranges::sort(single_files);
    CHECK(single_files == expected_single);

    const auto several_mods = scan_file_rule_preview(root, OptimizationMode::SeveralMods, btu::Game::SSE);
    REQUIRE(several_mods.has_value());
    CHECK(several_mods->skipped_archives == 0);
    const auto expected_several = std::vector<std::filesystem::path>{
        "meshes/armor.nif",
        "textures/interface.dds",
    };
    auto several_files = several_mods->files;
    std::ranges::sort(several_files);
    CHECK(several_files == expected_several);
}

TEST_CASE("file-rule preview reports an invalid input folder")
{
    auto temporary_directory = TemporaryDirectory{};
    const auto result        = scan_file_rule_preview(temporary_directory.path() / "missing",
                                               OptimizationMode::SingleMod,
                                               btu::Game::SSE);

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error() == "The selected input path is not a folder.");
}

TEST_CASE("Quick tasks use default settings and Customize can edit the selected rule")
{
    auto settings = Settings::make_base();

    auto custom_rule           = PerFileSettings::make_base(btu::Game::FNV);
    custom_rule.pattern        = Pattern::simple_rule({}, u8"dds", u8"All DDS textures");
    const auto custom_rule_key = custom_rule.pattern.text();
    settings.current_profile().prepend_per_file_settings(std::move(custom_rule));
    settings.gui.selected_pattern = custom_rule_key;

    settings.gui.workspace = Workspace::QuickTasks;
    CHECK(current_per_file_settings(settings).pattern.text() == k_default_pattern.text());
    CHECK(settings.gui.selected_pattern == custom_rule_key);

    settings.gui.workspace = Workspace::Customize;
    CHECK(current_per_file_settings(settings).pattern.text() == custom_rule_key);

    settings.gui.workspace = Workspace::QuickTasks;
    CHECK(current_per_file_settings(settings).pattern.text() == k_default_pattern.text());
}
} // namespace cao
