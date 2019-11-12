/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.hpp"

namespace CAO {
class BSAFile : public File
{
public:
    BSAFile();

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    void reset() override;

    const Qlibbsarch::BSArchiveAuto &getFile();

protected:
    std::unique_ptr<Qlibbsarch::BSArchiveAuto> _file;
};
} // namespace CAO
