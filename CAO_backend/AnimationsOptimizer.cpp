/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsOptimizer.h"

AnimationsOptimizer::AnimationsOptimizer()
{
    try
    {
        // Need to have memory allocated for the solver. Allocate 1mb for it.
        memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024 * 1024) );
        hkBaseSystem::init( memoryRouter, errorReport );

    } catch (const std::exception& e)
    {
        PLOG_ERROR << tr("An error occured while creating the animations optimizer. Animations won't be optimized.") + "\n" + QString(e.what());
    }
}

AnimationsOptimizer::~AnimationsOptimizer()
{

    hkBaseSystem::quit();
    hkMemoryInitUtil::quit();
}

void AnimationsOptimizer::convert(const QString &filePath, const hkPackFormat& pkFormat)
{
    try
    {
        bool flagsSpecified = false;
        hkSerializeUtil::SaveOptionBits flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_DEFAULT);

        if (!flagsSpecified && pkFormat == HKPF_DEFAULT)
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(hkSerializeUtil::SAVE_TEXT_FORMAT|hkSerializeUtil::SAVE_TEXT_NUMBERS);
        if (pkFormat == HKPF_XML || pkFormat == HKPF_TAGXML) // set text format to indicate xml
            flags = static_cast<hkSerializeUtil::SaveOptionBits>(flags | hkSerializeUtil::SAVE_TEXT_FORMAT);
        hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);

        hkVariant root;
        hkResource *resource;
        hkResult res;

        hkIstream istream(qPrintable(filePath));
        if(!istream.isOk())
            throw std::runtime_error("An error occured while loading this file.");

        hkStreamReader *reader = istream.getStreamReader();

        res = hkSerializeLoad(reader, root, resource);

        if (res != HK_SUCCESS)
            PLOG_WARNING << tr("File is not loadable: ") + filePath + '\n' + tr("It is probably already converted.");

        else
        {
            hkBool32 failed = true;
            if (root.m_object != nullptr)
            {
                hkOstream ostream(qPrintable(filePath.chopped(3) + "out.hkx"));

                if(!ostream.isOk())
                    throw std::runtime_error("An error occured while saving this file.");

                hkResult res = hkSerializeUtilSave(pkFormat, root, ostream, flags, packFileOptions);

                failed = (res != HK_SUCCESS);
                if (failed)
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
    catch (const std::exception& e)
    {
        PLOG_ERROR << "Unexpected exception occurred: " + QString(e.what()) + "\nwhile processing: " + filePath;
    }

    if(QFile::exists(filePath.chopped(3) + "out.hkx"))
    {
        QFile::remove(filePath);
        QFile::rename(filePath.chopped(3) + "out.hkx", filePath);
    }
}
