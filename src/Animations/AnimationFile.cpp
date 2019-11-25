/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "AnimationFile.hpp"

namespace CAO {
AnimationFile::AnimationFile()

{
    try
    {
        _file = std::make_unique<AnimationResource>();

        // Need to have memory allocated for the solver. Allocate 1mb for it.
        _memoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator,
                                                      hkMemorySystem::FrameInfo(1024 * 1024));
        hkBaseSystem::init(_memoryRouter, errorReport);

        LoadDefaultRegistry();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR
            << QString("An error occured while creating the animations optimizer. Animations won't be optimized.\n%1")
                   .arg(e.what());
    }
}

AnimationFile::~AnimationFile()
{
    hkBaseSystem::quit();
    hkMemoryInitUtil::quit();
}

void AnimationFile::reset()
{
    _file.reset(new AnimationResource);
    _optimizedCurrentFile = false;
    _filename.clear();
}

bool AnimationFile::setFile(Resource &file, bool optimizedFile)
{
    auto havok = dynamic_cast<AnimationResource *>(&file);
    if (!havok)
        return false;
    _optimizedCurrentFile |= optimizedFile;
    _file.reset(&file);
    return true;
}

int AnimationFile::loadFromDisk(const QString &filePath)
{
    try
    {
        auto havok = static_cast<AnimationResource *>(&*_file);

        hkIstream istream(qPrintable(filePath));
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

        _filename = filePath;

        return 0;
    }
    catch (...)
    {
        return 4;
    }
}

int AnimationFile::saveToDisk(const QString &filePath) const
{
    try
    {
        auto havok = static_cast<AnimationResource *>(&*_file);

        QString outPath = filePath;
        if (QFile::exists(outPath))
            filePath.chopped(3) + "out.hkx";

        hkOstream ostream(qPrintable(outPath));

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

        if (outPath != filePath && QFile::exists(outPath))
        {
            QFile::remove(filePath);
            if (!QFile::rename(outPath, filePath))
                return 3;
        }
        return 0;
    }
    catch (...)
    {
        return 4;
    }
}
} // namespace CAO
