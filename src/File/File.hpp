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

    void setInputFilePath(const QString &filePath);
    const QString &getInputFilePath() const;

    void setOutputFilePath(const QString &dirPath);
    const QString &getOutputFilePath() const;

    virtual int loadFromDisk();
    virtual int loadFromDisk(const QString &filePath) = 0;

    virtual int saveToDisk() const;
    virtual int saveToDisk(const QString &filePath) const = 0;

    virtual int loadFromMemory(const void *pSource, size_t size, const QString &fileName) = 0;
    virtual int saveToMemory(std::iostream &ostr) const                                   = 0;

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

        file_ = std::move(file);
        optimizedCurrentFile_ |= optimizedFile;
        isLoaded_ = true;
        return true;
    }

    void resetHelper()
    {
        isLoaded_             = false;
        optimizedCurrentFile_ = false;

        inputFilePath_.clear();
        outputFilePath_.clear();

        file_.release();
    }

    template<class T>
    void loadHelper(const QString &filename)
    {
        isLoaded_             = true;
        optimizedCurrentFile_ = false;

        setInputFilePath(filename);

        file_.reset(new T);
    }

    [[nodiscard]] bool saveToDiskHelper(const QString &filename) const
    {
        if (!isLoaded())
            return false;

        auto dir = QFileInfo(filename).dir();
        return dir.mkpath(dir.path());
    }

    [[nodiscard]] bool saveToMemoryHelper() const { return isLoaded(); }

private:
    void matchSettings();

    std::unique_ptr<Resource> file_;

    QString inputFilePath_;
    QString outputFilePath_;

    bool isLoaded_             = false;
    bool optimizedCurrentFile_ = false;

    PatternSettings patternSettings_;
};
} // namespace CAO
