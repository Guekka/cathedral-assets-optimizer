#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Commands/CommandBook.hpp"
#include "Commands/Textures/TextureConvert.hpp"
#include "File/Textures/TextureFile.hpp"
#include "Settings/Settings.hpp"
#include "doctest.h"
#include "pch.hpp"

using namespace CAO;

static std::unique_ptr<TextureFile> getStandardTextureFile(const bool optimizedFile = true,
                                                           const DXGI_FORMAT &format = DXGI_FORMAT_A8_UNORM)
{
    auto textureResource = new TextureResource;
    auto file = std::make_unique<TextureFile>();
    textureResource->Initialize2D(format, 16, 16, 1, 1);
    textureResource->origFormat = format;
    file->setFile(*textureResource, optimizedFile);

    //Checking that the file is valid
    auto res = dynamic_cast<const TextureResource *>(&file->getFile());
    REQUIRE(res->GetImages());

    return file;
}

SCENARIO("Saving and loading a dds file to the disk")
{
    GIVEN("A valid TextureFile and a valid path")
    {
        auto file = getStandardTextureFile(false);
        WHEN("The file is saved to disk")
        {
            const QString filePath = QDir::currentPath() + "/image.dds";
            file->saveToDisk(filePath);
            THEN("The file should be correctly saved") { CHECK(QFile::exists(filePath)); }
            WHEN("The file is loaded from disk")
            {
                THEN("The loaded file should be identical to the saved file")
                {
                    //We assume here here the loadFromDisk function works correctly
                    TextureFile file2;
                    CHECK(file2.loadFromDisk(filePath) == 0);
                    auto info1 = dynamic_cast<const TextureResource *>(&file->getFile())->GetMetadata();
                    auto info2 = dynamic_cast<const TextureResource *>(&file2.getFile())->GetMetadata();
                    CHECK(info1.width == info2.width);
                    CHECK(info1.height == info2.height);
                    CHECK(info1.depth == info2.depth);
                    CHECK(info1.format == info2.format);
                    CHECK(info1.arraySize == info2.arraySize);
                    CHECK(info1.mipLevels == info2.mipLevels);
                }
            }
        }
    }
}

SCENARIO("Optimizing a texture file")
{
    GIVEN("A valid TextureFile")
    {
        TextureFile file;
        WHEN("The TextureFile is unchanged")
        {
            THEN("optimizedCurrentFile should be false") { CHECK(file.optimizedCurrentFile() == false); }
        }
        WHEN("The TextureFile is changed with 'optimizedFile' set to false")
        {
            auto file2 = getStandardTextureFile(false);
            THEN("optimizedCurrentFile should be false") { CHECK(file2->optimizedCurrentFile() == false); }
        }
        WHEN("The TextureFile is changed with default arguments")
        {
            auto file3 = getStandardTextureFile();
            THEN("optimizedCurrentFile should be true") { CHECK(file3->optimizedCurrentFile() == true); }
        }
    }
}

SCENARIO("Converting a texture")
{
    CommandBook book;
    auto &convert = *book.getCommand<TextureConvert>();
    Settings sets;

    GIVEN("An unmodified image using an incompatible format, and settings with necessary opt enabled")
    {
        auto file = getStandardTextureFile(false, DXGI_FORMAT_B5G6R5_UNORM);
        sets.setValue(StandardSettings::bTexturesNecessary, true);

        WHEN("The file is scanned")
        {
            auto isApplicable = convert.isApplicable(*file, sets);
            CHECK(isApplicable == true);
        }
    }
    GIVEN("An unmodified image using a compatible format, and settings with necessary opt enabled")
    {
        auto file = getStandardTextureFile(false);
        sets.setValue(StandardSettings::bTexturesNecessary, true);

        WHEN("The file is scanned")
        {
            auto isApplicable = convert.isApplicable(*file, sets);
            CHECK(isApplicable == false);
        }
    }
    GIVEN("An unmodified image using a incompatible format, and settings with necessary opt disabled")
    {
        auto file = getStandardTextureFile(false, DXGI_FORMAT_B5G6R5_UNORM);
        sets.setValue(StandardSettings::bTexturesNecessary, false);

        WHEN("The file is scanned")
        {
            auto isApplicable = convert.isApplicable(*file, sets);
            CHECK(isApplicable == false);
        }
    }
    GIVEN("A modified image using a compatible format, and settings with necessary opt enabled")
    {
        auto file = getStandardTextureFile(true);
        sets.setValue(StandardSettings::bTexturesNecessary, true);

        WHEN("The file is scanned")
        {
            auto isApplicable = convert.isApplicable(*file, sets);
            CHECK(isApplicable == true);
        }
    }
    GIVEN("A modified image using the same format as the output format, and settings with necessary opt "
          "enabled")
    {
        auto file = getStandardTextureFile(true, sets.getMandatoryValue<DXGI_FORMAT>(AdvancedSettings::eTexturesFormat));
        sets.setValue(StandardSettings::bTexturesNecessary, true);

        WHEN("The file is scanned")
        {
            auto isApplicable = convert.isApplicable(*file, sets);
            CHECK(isApplicable == false);
        }
    }
}
