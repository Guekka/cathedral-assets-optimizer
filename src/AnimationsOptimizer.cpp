/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsOptimizer.h"

AnimationsOptimizer::AnimationsOptimizer()
{
    try
    {
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

AnimationsOptimizer::~AnimationsOptimizer()
{
    hkBaseSystem::quit();
    hkMemoryInitUtil::quit();
}

void AnimationsOptimizer::convert(const QString &filePath, const hkPackFormat &pkFormat) const
{
    try
    {

        auto flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_DEFAULT);

        if (pkFormat == HKPF_DEFAULT)
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_TEXT_FORMAT
                                                                 | hkSerializeUtil::SAVE_TEXT_NUMBERS);
        if (pkFormat == HKPF_XML || pkFormat == HKPF_TAGXML) // set text format to indicate xml
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(flags | hkSerializeUtil::SAVE_TEXT_FORMAT);

        auto packFileOptions = GetWriteOptionsFromFormat(pkFormat);

        hkIstream istream(qPrintable(filePath));
        if (!istream.isOk())
            throw std::runtime_error("An error occured while loading this file.");

        hkStreamReader *reader = istream.getStreamReader();
        hkVariant root;
        hkResource *resource;

        hkResult res = hkSerializeLoad(reader, root, resource);

        if (res)
            PLOG_WARNING << QString("File is not loadable: %1\nIt is probably already converted.").arg(filePath);
        else
        {
            if (root.m_object != nullptr)
            {
                hkOstream ostream(qPrintable(filePath.chopped(3) + "out.hkx"));

                if (!ostream.isOk())
                    throw std::runtime_error("An error occured while saving this file.");

                res = hkSerializeUtilSave(pkFormat, root, ostream, flags, packFileOptions);

                if (res)
                {
                    PLOG_ERROR << "Failed to save file: " + filePath;
                }
            }
            else
                PLOG_ERROR << "Failed to load file: " + filePath;
        }
        if (resource != nullptr)
            resource->removeReference();
    }
    catch (const std::exception &e)
    {
        PLOG_ERROR << QString("Unexpected exception occurred: '%1' while processing: %2").arg(e.what(), filePath);
    }

    if (QFile::exists(filePath.chopped(3) + "out.hkx"))
    {
        QFile::remove(filePath);
        QFile::rename(filePath.chopped(3) + "out.hkx", filePath);
    }
}
