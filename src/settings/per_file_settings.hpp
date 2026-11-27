/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <btu/common/path.hpp>
#include <btu/hkx/anim.hpp>
#include <btu/nif/optimize.hpp>
#include <btu/tex/optimize.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace cao {
class Pattern
{
    struct Regex
    {
        std::regex regex;
        std::u8string original;
    };

public:
    struct SimpleRule
    {
        std::u8string folder;
        std::u8string extension;
    };

    enum class Type : std::uint8_t
    {
        Wildcard,
        Regex,
        Simple,
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
            case Type::Simple:
                throw std::invalid_argument("Simple rules must be constructed from conditions");
        }
    }

    [[nodiscard]] static auto simple_rule(std::u8string folder,
                                          std::u8string extension,
                                          std::u8string name = {}) -> Pattern
    {
        folder    = normalize_folder(std::move(folder));
        extension = normalize_extension(std::move(extension));
        if (folder.empty() && extension.empty())
            throw std::invalid_argument("A simple rule must have a folder or file extension");

        Pattern pattern{SimpleRule{.folder = std::move(folder), .extension = std::move(extension)}};
        pattern.name_ = std::move(name);
        return pattern;
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
            [&str](const SimpleRule &rule) { return matches_simple_rule(str, rule); },
        };

        return std::visit(visitor, pattern_);
    }

    [[nodiscard]] auto text() const noexcept -> std::u8string
    {
        assert(!pattern_.valueless_by_exception());

        const auto visitor = btu::common::Overload{
            [](const std::u8string &pattern) { return pattern; },
            [](const Regex &regex) { return regex.original; },
            [](const SimpleRule &rule) { return simple_rule_key(rule); },
        };

        return std::visit(visitor, pattern_);
    }

    [[nodiscard]] auto type() const noexcept -> Type
    {
        assert(!pattern_.valueless_by_exception());
        if (std::holds_alternative<std::u8string>(pattern_))
            return Type::Wildcard;
        if (std::holds_alternative<Regex>(pattern_))
            return Type::Regex;
        return Type::Simple;
    }

    [[nodiscard]] auto name() const noexcept -> std::u8string_view { return name_; }

    void set_name(std::u8string name) noexcept { name_ = std::move(name); }

    [[nodiscard]] auto simple_rule() const noexcept -> const SimpleRule *
    {
        return std::get_if<SimpleRule>(&pattern_);
    }

    NLOHMANN_JSON_SERIALIZE_ENUM(
        Type, {{Type::Wildcard, "wildcard"}, {Type::Regex, "regex"}, {Type::Simple, "simple"}})

    [[maybe_unused]] friend void to_json(nlohmann::json &j, const Pattern &p)
    {
        j["pattern"] = p.text();
        j["type"]    = p.type();
        if (!p.name_.empty())
            j["name"] = p.name_;
        if (const auto *rule = p.simple_rule())
        {
            j["folder"]    = rule->folder;
            j["extension"] = rule->extension;
        }
    }

    [[maybe_unused]] friend void from_json(const nlohmann::json &j, Pattern &p)
    {
        const auto type = j.at("type").get<Type>();
        if (type == Type::Simple)
        {
            p = Pattern::simple_rule(j.at("folder").get<std::u8string>(),
                                     j.at("extension").get<std::u8string>());
        }
        else
        {
            p = Pattern{j.at("pattern").get<std::u8string>(), type};
        }

        if (j.contains("name"))
            p.name_ = j.at("name").get<std::u8string>();
    }

private:
    explicit Pattern(SimpleRule rule)
        : pattern_(std::move(rule))
    {
    }

    [[nodiscard]] static auto normalize_folder(std::u8string folder) -> std::u8string
    {
        std::ranges::replace(folder, u8'\\', u8'/');
        if (std::ranges::any_of(folder, [](const char8_t character) {
                return character == u8'*' || character == u8'?' || character == u8'[' || character == u8']';
            }))
        {
            throw std::invalid_argument("Folder paths cannot contain wildcard characters");
        }

        if ((!folder.empty() && folder.front() == u8'/') || (folder.size() > 1 && folder[1] == u8':'))
            throw std::invalid_argument("The folder must be relative to the input files");

        auto normalized = std::filesystem::path{folder}.lexically_normal().generic_u8string();
        if (normalized == u8".")
            return {};
        while (!normalized.empty() && normalized.back() == u8'/')
            normalized.pop_back();

        for (const auto &component : std::filesystem::path{normalized})
        {
            const bool is_parent = component == std::filesystem::path{u8".."};
            if (is_parent)
                throw std::invalid_argument("The folder cannot go above the input files");
        }

        return normalized;
    }

    [[nodiscard]] static auto normalize_extension(std::u8string extension) -> std::u8string
    {
        if (extension.empty())
            return extension;

        if (extension.front() != u8'.')
            extension.insert(extension.begin(), u8'.');

        if (extension.size() == 1 || std::ranges::any_of(extension, [](const char8_t character) {
                return character == u8'/' || character == u8'\\' || character == u8'*' || character == u8'?'
                       || character == u8'[' || character == u8']' || character == u8' '
                       || character == u8'\t' || character == u8'\r' || character == u8'\n';
            }))
        {
            throw std::invalid_argument("The extension must be a suffix, without a path or wildcard");
        }

        return extension;
    }

    [[nodiscard]] static auto simple_rule_key(const SimpleRule &rule) -> std::u8string
    {
        const auto append_size = [](std::u8string &text, const std::size_t size) {
            const auto digits = std::to_string(size);
            for (const char digit : digits)
                text.push_back(static_cast<char8_t>(digit));
        };

        std::u8string result{u8"@simple-rule:"};
        append_size(result, rule.folder.size());
        result.push_back(u8':');
        result += rule.folder;
        result.push_back(u8':');
        append_size(result, rule.extension.size());
        result.push_back(u8':');
        result += rule.extension;
        return result;
    }

    [[nodiscard]] static auto ascii_lower(const char8_t character) noexcept -> char8_t
    {
        if (character >= u8'A' && character <= u8'Z')
            return static_cast<char8_t>(character - u8'A' + u8'a');
        return character;
    }

    [[nodiscard]] static auto equals_ignore_ascii_case(const std::u8string_view left,
                                                       const std::u8string_view right) noexcept -> bool
    {
        if (left.size() != right.size())
            return false;

        for (std::size_t index = 0; index < left.size(); ++index)
        {
            if (ascii_lower(left[index]) != ascii_lower(right[index]))
                return false;
        }

        return true;
    }

    [[nodiscard]] static auto matches_simple_rule(const std::u8string &path,
                                                  const SimpleRule &rule) noexcept -> bool
    {
        if (!rule.folder.empty())
        {
            bool folder_found = false;
            for (std::size_t position = 0; position + rule.folder.size() <= path.size(); ++position)
            {
                if (position != 0 && path[position - 1] != u8'/')
                    continue;

                const auto folder_end = position + rule.folder.size();
                if (folder_end < path.size() && path[folder_end] != u8'/')
                    continue;

                if (equals_ignore_ascii_case(std::u8string_view{path}.substr(position, rule.folder.size()),
                                             rule.folder))
                {
                    folder_found = true;
                    break;
                }
            }

            if (!folder_found)
                return false;
        }

        if (rule.extension.empty())
            return true;

        const auto filename_start = path.find_last_of(u8'/');
        const auto filename       = std::u8string_view{path}.substr(
            filename_start == std::u8string::npos ? 0 : filename_start + 1);
        return filename.size() >= rule.extension.size()
               && equals_ignore_ascii_case(filename.substr(filename.size() - rule.extension.size()),
                                           rule.extension);
    }

    std::variant<std::u8string, Regex, SimpleRule> pattern_;
    std::u8string name_;
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
