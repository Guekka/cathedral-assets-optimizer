/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <QFile>

#include "File/Animations/AnimationFile.hpp"
#include "Utils/Algorithms.hpp"

namespace CAO {

bool AnimationFile::setFile(Resource &&file, bool optimizedFile)
{
    return setFileHelper<Resources::Animation>(std::move(file), optimizedFile);
}

int AnimationFile::loadFromDisk(const QString &filePath)
{
    loadHelper<Resources::Animation>(filePath);
    return 0;
}

int AnimationFile::saveToDisk(const QString &filePath)
{
    if (!saveToDiskHelper(filePath))
        return 5;
    return 0;
}

int AnimationFile::loadFromMemory(const void *, size_t, const QString &fileName)
{
    loadHelper<Resources::Animation>(fileName);
    return 1;
}

int AnimationFile::saveToMemory(std::vector<std::byte> &)
{
    if (!saveToMemoryHelper())
        return 1;

    return 1;
}

} // namespace CAO
