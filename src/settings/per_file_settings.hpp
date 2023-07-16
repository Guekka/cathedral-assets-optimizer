/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "base_types.hpp"

#include <btu/hkx/anim.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>

#include <optional>
#undef __pre // TODO: move that to btu/common
#include <regex>

namespace cao {
class Pattern
{
public:
    enum class Type
    {
        Wildcard,
        Regex,
    };

    Pattern(std::u8string pattern, Type type)
        : pattern_(std::move(pattern))
        , type_(type)
    {
    }

    [[nodiscard]] auto matches(const std::filesystem::path &path) const noexcept -> bool
    {
        switch (type_)
        {
            case Type::Wildcard: return btu::common::str_match(path.u8string(), pattern_);
            case Type::Regex:
                return std::regex_match(btu::common::as_ascii_string(path.u8string()),
                                        std::regex(btu::common::as_ascii_string(pattern_)));
        }
        assert(false && "Pattern::matches: unreachable");
    }

private:
    std::u8string pattern_;
    Type type_;
};

struct PerFileSettings
{
    btu::tex::Settings tex;
    btu::nif::Settings nif;
    std::optional<btu::Game> hkx_target;

    std::vector<Pattern> patterns;
};

} // namespace cao
