/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "File/File.hpp"

namespace CAO {
class ModFolder
{
public:
    explicit ModFolder(const QString &inDir, const QString &bsaExtension, const QString &outDir);
    ModFolder(const ModFolder &) = delete;
    ModFolder(ModFolder &&)      = default;

    ModFolder &operator=(const ModFolder &) = delete;
    ModFolder &operator=(ModFolder &&) = default;

    void load();
    bool hasNext() const;

    /// \note UB if hasNext == false
    std::unique_ptr<File> consume();

    size_t processedFileCount() const;
    size_t totalFileCount() const;

    QString name() const;

    QString inPath() const;
    QString outPath() const;

private:
    std::vector<std::unique_ptr<File>> bsas_;
    std::vector<std::unique_ptr<File>> files_;

    bool BSAExhausted_ = false;

    QString inDir_;
    QString outDir_;

    QString bsaExtension_;

    size_t processedFileCount_ = 0;
    size_t totalFileCount_     = 0;
};
} // namespace CAO
