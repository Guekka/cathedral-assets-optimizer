/* Copyright (C) 2026 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "settings/base_types.hpp"

#include <btu/common/games.hpp>
#include <btu/common/path.hpp>

#include <cstddef>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace cao {
struct FileRulePreviewScan
{
    std::vector<std::filesystem::path> files;
    std::size_t skipped_archives = 0;
};

using FileRulePreviewResult = std::expected<FileRulePreviewScan, std::string>;

[[nodiscard]] auto scan_file_rule_preview(const btu::Path &input_path,
                                          OptimizationMode mode,
                                          btu::Game target_game) -> FileRulePreviewResult;
} // namespace cao
