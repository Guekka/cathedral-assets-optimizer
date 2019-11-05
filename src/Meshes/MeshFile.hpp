/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File.hpp"
#include "pch.hpp"

namespace CAO {
class MeshFile : public File
{
public:
    MeshFile();

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    const NifFile &getFile() const;
    void setFile(NifFile &file);

    void reset() override;

protected:
    std::unique_ptr<NifFile> _file;
};
} // namespace CAO
