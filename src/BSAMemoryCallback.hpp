/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "File/FileFactory.hpp"
#include "MainOptimizer.hpp"
#include "pch.hpp"

namespace CAO {
inline libbsarch::transform_callback BSAMemoryCallback(const QString &bsaExt,
                                                       MainOptimizer &optimizer,
                                                       bool dryRun = false)
{
    return [bsaExt, &optimizer, dryRun](const libbsarch::fs::path &relativePath,
                                        libbsarch::extracted_data &&blob) {
        const auto file = makeFile(QString::fromStdString(relativePath.string()), bsaExt);
        if (!file)
            return std::vector<std::byte>{};

        std::vector<std::byte> out;
        const MainOptimizer::MemoryData result{blob.get_buffer(), blob.get_size(), &out};

        optimizer.process(*file, dryRun, result);
        return out;
    };
}
} // namespace CAO
