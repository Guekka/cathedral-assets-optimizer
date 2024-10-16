/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <btu/common/path.hpp>
#include <btu/hkx/anim.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>

#include <optional>
#include <regex>

namespace cao {
class Pattern
{
    struct Regex
    {
        std::regex regex;
        std::u8string original;
    };

public:
    enum class Type : std::uint8_t
    {
        Wildcard,
        Regex,
    };

    explicit Pattern(std::u8string pattern, const Type type = Type::Wildcard)
    {
        switch (type)
        {
            case Type::Wildcard:
            {
                pattern_ = BTU_MOV(pattern);
                break;
            }
            case Type::Regex:
            {
                pattern_ = Regex{.regex    = std::regex{btu::common::as_ascii_string(pattern),
                                                     std::regex::optimize | std::regex::icase},
                                 .original = pattern};
                break;
            }
        }
    }

    [[nodiscard]] auto matches(const std::filesystem::path &path) const noexcept -> bool
    {
        assert(!pattern_.valueless_by_exception());

        constexpr auto canonize_path = btu::common::make_path_canonizer(u8"");

        auto str = canonize_path(BTU_MOV(path));

        const auto visitor = btu::common::Overload{
            [&str](const std::u8string &pattern) { return btu::common::str_match(str, pattern); },
            [&str](const Regex &regex) {
                return std::regex_match(btu::common::as_ascii_string(str), regex.regex);
            },
        };

        return std::visit(visitor, pattern_);
    }

    [[nodiscard]] auto text() const noexcept -> std::u8string
    {
        assert(!pattern_.valueless_by_exception());

        const auto visitor = btu::common::Overload{
            [](const std::u8string &pattern) { return pattern; },
            [](const Regex &regex) { return regex.original; },
        };

        return std::visit(visitor, pattern_);
    }

    [[nodiscard]] auto type() const noexcept -> Type
    {
        assert(!pattern_.valueless_by_exception());
        return pattern_.index() == 0 ? Type::Wildcard : Type::Regex;
    }

    NLOHMANN_JSON_SERIALIZE_ENUM(Type, {{Type::Wildcard, "wildcard"}, {Type::Regex, "regex"}})

    [[maybe_unused]] friend void to_json(nlohmann::json &j, const Pattern &p)
    {
        j["pattern"] = p.text();
        j["type"]    = p.pattern_.index() == 0 ? Type::Wildcard : Type::Regex;
    }

    [[maybe_unused]] friend void from_json(const nlohmann::json &j, Pattern &p)
    {
        p = Pattern{j.at("pattern").get<std::u8string>(), j.at("type").get<Type>()};
    }

private:
    std::variant<std::u8string, Regex> pattern_;
};

const auto k_default_pattern = Pattern{u8"*"}; // NOLINT(cert-err58-cpp)

enum class OptimizeType : std::uint8_t
{
    None,
    DryRun,
    Normal,
    Forced,
};

struct PerFileSettings
{
    bool pack = true;

    OptimizeType tex_optimize = OptimizeType::Normal;
    btu::tex::Settings tex    = btu::tex::Settings::get(btu::Game::SSE);

    OptimizeType nif_optimize = OptimizeType::Normal;
    btu::nif::Settings nif    = btu::nif::Settings::get(btu::Game::SSE);

    OptimizeType hkx_optimize = OptimizeType::Normal;
    btu::Game hkx_target      = btu::Game::SSE;

    Pattern pattern = k_default_pattern;

    [[nodiscard]] auto matches(const std::filesystem::path &path) const noexcept -> bool
    {
        return pattern.matches(path);
    }

    [[nodiscard]] static auto make_base(btu::Game game) noexcept -> PerFileSettings
    {
        PerFileSettings settings{
            .tex        = btu::tex::Settings::get(game),
            .nif        = btu::nif::Settings::get(game),
            .hkx_target = game,
        };

        settings.pattern = k_default_pattern;

        return settings;
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    PerFileSettings, pack, tex_optimize, tex, nif_optimize, nif, hkx_optimize, hkx_target, pattern)

} // namespace cao
