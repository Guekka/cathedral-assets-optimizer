/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ModFolder.hpp"
#include "File/FileFactory.hpp"

namespace CAO {
ModFolder::ModFolder(const QString &inDir, const QString &bsaExtension, const QString &outDir)
    : inDir_(inDir)
    , outDir_(outDir)
    , bsaExtension_(bsaExtension)
{
}

bool ModFolder::hasNext() const
{
    return !bsas_.empty() || !files_.empty();
}

std::unique_ptr<File> ModFolder::consume()
{
    auto helper = [this](auto &vec) {
        processedFileCount_++;
        auto file = std::move(vec.back());
        vec.pop_back();
        return std::move(file);
    };

    std::unique_ptr<File> res;

    if (!bsas_.empty())
        res = helper(bsas_);
    else
        res = helper(files_);

    //If the BSAs were extracted, new files could have appeared
    if (!BSAExhausted_ && bsas_.empty())
    {
        BSAExhausted_ = true;
        load();
    }

    return res;
}

size_t ModFolder::processedFileCount() const
{
    return processedFileCount_;
}

size_t ModFolder::totalFileCount() const
{
    return totalFileCount_;
}

QString ModFolder::name() const
{
    return QDir(inDir_).dirName();
}

QString ModFolder::inPath() const
{
    return inDir_;
}

QString ModFolder::outPath() const
{
    return outDir_;
}

void ModFolder::load()
{
    bsas_.clear();
    files_.clear();

    //Typical amount of medium mod
    bsas_.reserve(5);
    files_.reserve(1'000);

    auto isBSA = [this](const QString &fileName) {
        return fileName.endsWith(bsaExtension_, Qt::CaseInsensitive);
    };

    QDirIterator it(inDir_, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();

        if (isBSA(it.fileName()) && BSAExhausted_)
            continue; //BSAs were already processed. New ones cannot have appeared

        auto file = makeFile(it.filePath(), bsaExtension_);

        if (file)
        {
            const auto &inputPath = file->getInputFilePath();
            file->setOutputFilePath(QString{inputPath}.replace(inPath(), outPath()));

            auto &vec = isBSA(it.fileName()) ? bsas_ : files_;
            vec.emplace_back(std::move(file));
        }
    }
    BSAExhausted_ |= bsas_.empty();

    if (!totalFileCount_)
        totalFileCount_ = bsas_.size() + files_.size();

    //Memory optimization
    bsas_.shrink_to_fit();
    files_.shrink_to_fit();
}

} // namespace CAO
