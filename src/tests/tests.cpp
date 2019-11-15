#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Textures/Texture.hpp"
#include "doctest.h"
#include "pch.hpp"

using namespace CAO;

SCENARIO("Saving and loading a dds file to the disk")
{
    GIVEN("A valid TextureFile and a valid path")
    {
        auto image = std::make_unique<TextureResource>();
        TextureFile file;
        image->Initialize2D(DXGI_FORMAT_A8_UNORM, 16, 16, 1, 1);
        //Checking that the file is valid
        REQUIRE(image->GetImages());

        file.setFile(*image.release(), false);
        WHEN("The file is saved to disk")
        {
            const QString filePath = QDir::currentPath() + "/image.dds";
            file.saveToDisk(filePath);
            THEN("The file should be correctly saved") { CHECK(QFile::exists(filePath)); }
            WHEN("The file is loaded from disk")
            {
                THEN("The loaded file should be identical to the saved file")
                {
                    //We assume here here the loadFromDisk function works correctly
                    TextureFile file2;
                    CHECK(file2.loadFromDisk(filePath) == 0);
                    auto info1 = dynamic_cast<const TextureResource *>(&file.getFile())->GetMetadata();
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
            auto image = std::make_unique<TextureResource>();
            image->Initialize2D(DXGI_FORMAT_A8_UNORM, 16, 16, 1, 1);
            file.setFile(*image.release(), false);
            THEN("optimizedCurrentFile should be false") { CHECK(file.optimizedCurrentFile() == false); }
        }
        WHEN("The TextureFile is changed with default arguments")
        {
            auto image = std::make_unique<TextureResource>();
            image->Initialize2D(DXGI_FORMAT_A8_UNORM, 16, 16, 1, 1);
            file.setFile(*image.release());
            THEN("optimizedCurrentFile should be true") { CHECK(file.optimizedCurrentFile() == true); }
        }
    }
}

SCENARIO("Converting a texture")
{
    GIVEN("An image compressed using an incompatible format")
    {
        auto image = std::make_unique<TextureResource>();
        TextureFile file;
        image->Initialize2D(DXGI_FORMAT_A8_UNORM, 16, 16, 1, 1);
        file.setFile(*image.release(), false);
    }
}
