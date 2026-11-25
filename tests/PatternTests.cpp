/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "settings/per_file_settings.hpp"
#include "settings/profile.hpp"

#include <doctest/doctest.h>

namespace cao {
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
    CHECK_THROWS_AS(Pattern::simple_rule(u8"../textures", u8"dds"), std::invalid_argument);
    CHECK_THROWS_AS(Pattern::simple_rule(u8"/textures", u8"dds"), std::invalid_argument);
    CHECK_THROWS_AS(Pattern::simple_rule(u8"textures/*", u8"dds"), std::invalid_argument);
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
    auto wildcard = Pattern{u8"*.nif"};
    auto regex    = Pattern{u8".*\\.nif", Pattern::Type::Regex};
    nlohmann::json{{"pattern", "*.dds"}, {"type", "wildcard"}}.get_to(wildcard);
    nlohmann::json{{"pattern", R"(.*\.dds)"}, {"type", "regex"}}.get_to(regex);

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
} // namespace cao
