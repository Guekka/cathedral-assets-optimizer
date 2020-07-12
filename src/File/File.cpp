/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/File.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

void File::setInputFilePath(const QString &filePath)
{
    if (inputFilePath_ == filePath)
        return;

    inputFilePath_ = filePath;
    matchSettings();
}

const QString &File::getInputFilePath() const
{
    return inputFilePath_;
}

void File::setOutputFilePath(const QString &dirPath)
{
    outputFilePath_ = dirPath;
}

const QString &File::getOutputFilePath() const
{
    if (outputFilePath_.isEmpty())
        return inputFilePath_;

    return outputFilePath_;
}

int File::loadFromDisk()
{
    return loadFromDisk(getInputFilePath());
}

int File::saveToDisk() const
{
    return saveToDisk(getOutputFilePath());
}

bool File::optimizedCurrentFile() const
{
    return optimizedCurrentFile_;
}

void File::setOptimizedCurrentFile(const bool optimizedFile)
{
    optimizedCurrentFile_ |= optimizedFile;
}

const Resource &File::getFile() const
{
    return *file_;
}

Resource &File::getFile(const bool modifiedFile)
{
    setOptimizedCurrentFile(modifiedFile);
    return *file_;
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
    patternSettings_ = currentProfile().getSettings(getInputFilePath());
}

} // namespace CAO
