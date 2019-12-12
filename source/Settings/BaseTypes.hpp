/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

constexpr double GigaByte = 1000 * 1000 * 1000;

namespace CAO {
enum OptimizationMode { SingleMod = 0, SeveralMods = 1 };
} // namespace CAO

Q_DECLARE_METATYPE(DXGI_FORMAT)
Q_DECLARE_METATYPE(NiFileVersion)
Q_DECLARE_METATYPE(bsa_archive_type_e)
Q_DECLARE_METATYPE(CAO::OptimizationMode)
