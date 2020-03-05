/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/Resources.hpp"
#include "Settings/Settings.hpp"
#include "pch.hpp"

namespace CAO {
class File
{
public:
    const QString &getName() const;
    void setName(const QString &name);

    virtual int loadFromDisk(const QString &filePath) = 0;
    virtual int loadFromMemory(const void *pSource, const size_t &size, const QString &fileName);

    virtual int saveToDisk(const QString &filePath) const = 0;
    virtual int saveToMemory(const void *pSource, const size_t &size, const QString &fileName) const;

    bool optimizedCurrentFile() const;
    void setOptimizedCurrentFile(const bool optimizedFile);

    const Resource &getFile() const;
    Resource &getFile(const bool modifiedFile);

    virtual bool setFile(Resource &file, bool optimizedFile = true) = 0;
    virtual void reset() = 0;

    const GeneralSettings &generalSettings() const;
    const PatternSettings &patternSettings() const;

    virtual ~File() = default;

protected:
    template<class T>
    bool setFileHelper(Resource &file, bool optimizedFile)
    {
        auto convertedFile = dynamic_cast<T *>(&file);
        if (!convertedFile)
            return false;

        _file.reset(&file);
        _optimizedCurrentFile |= optimizedFile;
        return true;
    }

    template<class T>
    void resetHelper()
    {
        _filename.clear();
        _file.reset(new T);
        _optimizedCurrentFile = false;
        generalSettings_ = nullptr;
        patternSettings_ = nullptr;
    }

private:
    QString _filename;
    std::unique_ptr<Resource> _file;
    bool _optimizedCurrentFile = false;

    GeneralSettings *generalSettings_;
    PatternSettings *patternSettings_;

    void matchSettings();
};
} // namespace CAO
