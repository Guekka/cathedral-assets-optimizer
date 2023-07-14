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

namespace cao {
struct PerFileSettings
{
    btu::tex::Settings tex;
    btu::nif::Settings nif;
    std::optional<btu::Game> hkx_target;
};
} // namespace cao
