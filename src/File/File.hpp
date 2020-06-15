/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandType.hpp"
#include "File/Resources.hpp"
#include "Settings/GeneralSettings.hpp"
#include "Settings/PatternSettings.hpp"
#include "pch.hpp"

//TODO Only call once some init operations for files
namespace CAO {
class File
{
public:
    virtual ~File() = default;

    const QString &getName() const;
    void setName(const QString &name);

    virtual int loadFromDisk();
    virtual int loadFromDisk(const QString &filePath) = 0;

    virtual int saveToDisk() const;
    virtual int saveToDisk(const QString &filePath) const = 0;

    virtual int loadFromMemory(const void *pSource, const size_t &size, const QString &fileName);
    virtual int saveToMemory(const void *pSource, const size_t &size, const QString &fileName) const;

    bool optimizedCurrentFile() const;
    void setOptimizedCurrentFile(const bool optimizedFile);

    const Resource &getFile() const;
    Resource &getFile(const bool modifiedFile);

    virtual bool setFile(std::unique_ptr<Resource> file, bool optimizedFile = true) = 0;
    virtual void reset();

    bool isLoaded() const { return isLoaded_; }

    virtual CommandType type() = 0;

    const PatternSettings &patternSettings() const;

protected:
    template<class T>
    bool setFileHelper(std::unique_ptr<Resource> file, bool optimizedFile)
    {
        auto convertedFile = dynamic_cast<T *>(&*file);
        if (!convertedFile)
            return false;

        _file = std::move(file);
        _optimizedCurrentFile |= optimizedFile;
        isLoaded_ = true;
        return true;
    }

    void resetHelper()
    {
        isLoaded_ = false;
        _filename.clear();
        _optimizedCurrentFile = false;
        _file.release();
    }

    template<class T>
    void loadHelper(const QString &filename)
    {
        isLoaded_ = true;
        setName(filename);
        _file.reset(new T);
    }

private:
    QString _filename;
    bool isLoaded_ = false;

    std::unique_ptr<Resource> _file;
    bool _optimizedCurrentFile = false;

    PatternSettings patternSettings_;

    void matchSettings();
};
} // namespace CAO
