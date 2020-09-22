/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <cctype>
#include <string_view>

namespace wildcards {

struct pattern
{
    std::string_view v;
};

struct cards
{
    char any;
    char any_repeat;
    char set_begin;
    char set_end;
};

constexpr cards defaultCards{'?', '*', '[', ']'};

constexpr bool case_sensitive   = true;
constexpr bool case_insensitive = false;

constexpr bool isMatch(std::string_view str,
                       pattern strong_pattern,
                       bool case_sensitive = true,
                       cards cards         = defaultCards)
{
    // Empty pattern can only match with empty sting
    if (strong_pattern.v.empty())
        return str.empty();

    auto pat_it        = strong_pattern.v.begin();
    const auto pat_end = strong_pattern.v.end();

    auto str_it        = str.begin();
    const auto str_end = str.end();

    auto anyrep_pos_pat = pat_end;
    auto anyrep_pos_str = str_end;

    auto set_pos_pat = pat_end;

    while (str_it != str_end)
    {
        char current_pat = 0;
        char current_str = -1;
        if (pat_it != pat_end)
        {
            current_pat = case_sensitive ? *pat_it : std::tolower(*pat_it);
            current_str = case_sensitive ? *str_it : std::tolower(*str_it);
        }
        if (pat_it != pat_end && current_pat == cards.set_begin)
        {
            set_pos_pat = pat_it;
            ++pat_it;
        }
        else if (pat_it != pat_end && current_pat == cards.set_end)
        {
            set_pos_pat = pat_end;
            ++pat_it;
        }
        else if (set_pos_pat != pat_end)
        {
            if (current_pat == current_str)
            {
                set_pos_pat = pat_end;
                pat_it      = std::find(pat_it, pat_end, cards.set_end) + 1;
                ++str_it;
            }
            else
            {
                if (pat_it == pat_end)
                {
                    return false;
                }
                ++pat_it;
            }
        }
        else if (pat_it != pat_end && current_pat == current_str)
        {
            ++pat_it;
            ++str_it;
        }
        else if (pat_it != pat_end && current_pat == cards.any)
        {
            ++pat_it;
            ++str_it;
        }
        else if (pat_it != pat_end && current_pat == cards.any_repeat)
        {
            anyrep_pos_pat = pat_it;
            anyrep_pos_str = str_it;
            ++pat_it;
        }
        else if (anyrep_pos_pat != pat_end)
        {
            pat_it = anyrep_pos_pat + 1;
            str_it = anyrep_pos_str + 1;
            ++anyrep_pos_str;
        }
        else
        {
            return false;
        }
    }
    while (pat_it != pat_end)
    {
        const char cur = case_sensitive ? *pat_it : std::tolower(*pat_it);
        if (cur == cards.any_repeat)
            ++pat_it;
        else
            break;
    }
    return pat_it == pat_end;
}
} // namespace wildcards
