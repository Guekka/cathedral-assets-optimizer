/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"

class QDir;

namespace CAO {
class GeneralSettings;

namespace BSASplit {
std::vector<BSA> splitBSA(const QDir &dir, const GeneralSettings &generalSets);
}; // namespace BSASplit
} // namespace CAO
