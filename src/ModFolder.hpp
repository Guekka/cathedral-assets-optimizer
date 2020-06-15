/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "File/BSA/BSAFile.hpp"
#include "File/File.hpp"

namespace CAO {
class ModFolder
{
public:
    explicit ModFolder(const QString &dir, const QString &bsaExtension);
    ModFolder(const ModFolder &) = delete;
    ModFolder(ModFolder &&)      = default;

    ModFolder &operator=(const ModFolder &) = delete;
    ModFolder &operator=(ModFolder &&) = default;

    void load();
    bool hasNext() const;

    /// \note UB if hasNext == false
    std::unique_ptr<File> consume();

    size_t processedBSACount() const;
    size_t processedFileCount() const;

    size_t remainingBSACount() const;
    size_t remainingFileCount() const;

    QString path() const;
    QString name() const;

private:
    std::vector<std::unique_ptr<File>> bsas_;
    std::vector<std::unique_ptr<File>> files_;

    bool BSAExhausted = false;

    QString dir_;
    QString bsaExtension_;

    size_t processedBSACount_  = 0;
    size_t processedFileCount_ = 0;
};
} // namespace CAO
