/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/File.hpp"

namespace CAO {
class AnimationFile : public File
{
public:
    AnimationFile();
    ~AnimationFile();

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    bool setFile(std::unique_ptr<Resource> file, bool optimizedFile = true) override;

    void reset() override;

private:
    hkMemoryRouter *_memoryRouter;
};
} // namespace CAO
