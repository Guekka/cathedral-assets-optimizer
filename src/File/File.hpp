/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/CommandType.hpp"
#include "Resources.hpp"
#include "Settings/PatternSettings.hpp"

//TODO Only call once some init operations for files
namespace CAO {

class File
{
public:
    virtual ~File(); // = default

    void setInputFilePath(const QString &filePath);
    const QString &getInputFilePath() const;

    void setOutputFilePath(const QString &dirPath);
    const QString &getOutputFilePath() const;

    virtual int loadFromDisk();
    virtual int loadFromDisk(const QString &filePath) = 0;

    virtual int saveToDisk();
    virtual int saveToDisk(const QString &filePath) = 0;

    virtual int loadFromMemory(const void *pSource, size_t size, const QString &fileName) = 0;
    virtual int saveToMemory(std::vector<std::byte> &out)                                 = 0;

    bool optimizedCurrentFile() const;
    void setOptimizedCurrentFile(const bool optimizedFile);

    template<typename T>
    const T *getFile() const
    {
        if (!std::holds_alternative<T>(file_))
            return nullptr;
        return &std::get<T>(file_);
    }

    template<typename T>
    T *getFile(const bool modifiedFile)
    {
        if (!std::holds_alternative<T>(file_))
            return nullptr;

        setOptimizedCurrentFile(modifiedFile);
        return &std::get<T>(file_);
    }

    virtual bool setFile(Resource &&file, bool optimizedFile = true) = 0;

    virtual void reset();

    bool isLoaded() const { return isLoaded_; }

    virtual CommandType type() const = 0;

    const PatternSettings &patternSettings() const;

protected:
    template<class T>
    bool setFileHelper(Resource &&file, bool optimizedFile)
    {
        if (!std::holds_alternative<T>(file))
            return false;

        file_ = std::move(file);
        optimizedCurrentFile_ |= optimizedFile;
        isLoaded_ = true;
        return true;
    }

    void resetHelper();

    template<class T>
    void loadHelper(const QString &filename)
    {
        isLoaded_             = true;
        optimizedCurrentFile_ = false;

        setInputFilePath(filename);

        file_ = T();
    }

    [[nodiscard]] bool saveToDiskHelper(const QString &filename) const;
    [[nodiscard]] bool saveToMemoryHelper() const;

private:
    void matchSettings();

    Resource file_;

    QString inputFilePath_;
    QString outputFilePath_;

    bool isLoaded_             = false;
    bool optimizedCurrentFile_ = false;

    PatternSettings patternSettings_;
};

} // namespace CAO
