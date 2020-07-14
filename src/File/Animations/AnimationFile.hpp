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

    int loadFromDisk(const QString &filePath) override;
    int saveToDisk(const QString &filePath) const override;

    int loadFromMemory(const void *pSource, size_t size, const QString &fileName) override;
    int saveToMemory(std::vector<std::byte> &out) const override;

    bool setFile(std::unique_ptr<Resource> file, bool optimizedFile = true) override;

    CommandType type() override { return CommandType::Animation; }

private:
    hkMemoryRouter *_memoryRouter;
    static inline bool initialized_ = false;

    int commonLoadHelper(hkIstream &istream);
    int commonSaveHelper(hkOstream &ostream) const;
};
} // namespace CAO
