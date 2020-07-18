/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "File/Animations/AnimationFile.hpp"

namespace CAO {
AnimationFile::AnimationFile()

{
    if (initialized_)
        return;

    try
    {
        // Need to have memory allocated for the solver. Allocate 1mb for it.
        _memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator,
                                                      hkMemorySystem::FrameInfo(1024 * 1024));
        hkBaseSystem::init(_memoryRouter, errorReport);

        LoadDefaultRegistry();

        initialized_ = true;
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << QString("An error occured while creating the animations optimizer. Animations "
                              "won't be optimized.\n%1")
                          .arg(e.what());
    }
}


bool AnimationFile::setFile(std::unique_ptr<Resource> file, bool optimizedFile)
{
    return setFileHelper<AnimationResource>(std::move(file), optimizedFile);
}

int AnimationFile::loadFromDisk(const QString &filePath)
{
    loadHelper<AnimationResource>(filePath);

    hkIstream istream(qPrintable(filePath));
    return commonLoadHelper(istream);
}

int AnimationFile::saveToDisk(const QString &filePath) const
{
    if (!saveToDiskHelper(filePath))
        return 5;

    //Outputting to a temporarary file
    QString outPath = filePath;
    if (QFile::exists(outPath))
        outPath = outPath.chopped(3) + "out.hkx";

    hkOstream ostream(qPrintable(outPath));

    commonSaveHelper(ostream);

    if (outPath != filePath && QFile::exists(outPath))
    {
        QFile::remove(filePath);
        if (!QFile::rename(outPath, filePath))
            return 3;
    }

    return 0;
}

int AnimationFile::loadFromMemory(const void *pSource, size_t size, const QString &fileName)
{
    loadHelper<AnimationResource>(fileName);

    hkIstream istream(pSource, size);
    return commonLoadHelper(istream);
}

int AnimationFile::saveToMemory(std::vector<std::byte> &out) const
{
    if (!saveToMemoryHelper())
        return 1;

    hkArray<char, hkContainerHeapAllocator> buffer;
    buffer.reserve(1024 * 1024 * 1024); //1mb is enough for most cases
    hkOstream ostream(buffer);

    if (auto res = commonSaveHelper(ostream); res)
        return res;

    out.reserve(buffer.getSize());
    move_transform(buffer, std::back_inserter(out), [](char &&c) { return std::byte(c); });
}

int AnimationFile::commonLoadHelper(hkIstream &istream)
{
    try
    {
        auto havok = static_cast<AnimationResource *>(&getFile(false));

        if (!istream.isOk())
            return 1;

        hkStreamReader *reader = istream.getStreamReader();

        hkResult res = hkSerializeLoad(reader, havok->root, havok->resource);

        if (res)
            return 2;
        if (!havok->root.m_object)
            return 3;

        hkSerializeUtil::FormatDetails formatDetails;
        hkSerializeUtil::detectFormat(reader, formatDetails);
        havok->pkFormat = GetFormatFromLayout(formatDetails.m_layoutRules);

        return 0;
    }
    catch (...)
    {
        reset();
        return 4;
    }
}

int AnimationFile::commonSaveHelper(hkOstream &ostream) const
{
    try
    {
        auto havok = static_cast<const AnimationResource *>(&getFile());

        if (!ostream.isOk())
            return 1;

        auto flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_DEFAULT);

        if (havok->pkFormat == HKPF_DEFAULT)
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_TEXT_FORMAT
                                                                 | hkSerializeUtil::SAVE_TEXT_NUMBERS);
        if (havok->pkFormat == HKPF_XML || havok->pkFormat == HKPF_TAGXML) // set text format to indicate xml
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(flags | hkSerializeUtil::SAVE_TEXT_FORMAT);

        const auto packFileOptions = GetWriteOptionsFromFormat(havok->pkFormat);
        const auto res = hkSerializeUtilSave(havok->pkFormat, havok->root, ostream, flags, packFileOptions);

        if (res)
            return 2;

        if (havok->resource)
            havok->resource->removeReference();

        return 0;
    }
    catch (...)
    {
        return 4;
    }
}

} // namespace CAO
