/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/File.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

const QString &File::getName() const
{
    return _filename;
}

void File::setName(const QString &name)
{
    if (name == _filename)
        return;

    _filename = QDir::cleanPath(name);
    matchSettings();
}

int File::loadFromDisk()
{
    return loadFromDisk(_filename);
}

int File::saveToDisk() const
{
    return saveToDisk(_filename);
}

int File::saveToMemory(const void *pSource, const size_t &size, const QString &fileName) const
{
    assert(false && "Function 'File::saveToMemory' should never be called");
    return 1;
}

int File::loadFromMemory(const void *pSource, const size_t &size, const QString &fileName)
{
    assert(false && "Function 'File::saveToMemory' should never be called");
    return 1;
}

bool File::optimizedCurrentFile() const
{
    return _optimizedCurrentFile;
}

void File::setOptimizedCurrentFile(const bool optimizedFile)
{
    _optimizedCurrentFile |= optimizedFile;
}

const Resource &File::getFile() const
{
    return *_file;
}

Resource &File::getFile(const bool modifiedFile)
{
    setOptimizedCurrentFile(modifiedFile);
    return *_file;
}

void File::reset()
{
    resetHelper();
}

const PatternSettings &CAO::File::patternSettings() const
{
    return patternSettings_;
}

void File::matchSettings()
{
    patternSettings_ = currentProfile().getSettings(_filename);
}

} // namespace CAO
