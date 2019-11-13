#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Textures/Texture.hpp"
#include "doctest.h"
#include "pch.hpp"

using namespace CAO;

SCENARIO("Saving a dds file to the disk")
{
    GIVEN("A valid TextureFile and a valid path")
    {
        WHEN("The file is saved to disk")
        {
            THEN("The file should be correctly saved") {}
        }
    }
}

SCENARIO("Loading a dds file from the disk")
{
    SUBCASE("Modified file is correctly set after using TextureFile::setFile")
    {
        auto image = std::make_unique<DirectX::ScratchImage>();
        TextureFile file;
        CHECK(file.modifiedCurrentFile() == false);
        file.setFileUnmodified(image);
        CHECK(file.modifiedCurrentFile() == false);
        image = std::make_unique<DirectX::ScratchImage>();
        file.setFile(image);
        CHECK(file.modifiedCurrentFile() == true);
    }
    SUBCASE("Filesystem operations")
    {
        const QString filePath = QDir::currentPath() + "/image.dds";
        CAPTURE(filePath.toStdString());
        SUBCASE("Saving file to disk")
        {
            auto image = std::make_unique<DirectX::ScratchImage>();
            image->Initialize2D(DXGI_FORMAT_A8_UNORM, 16, 16, 1, 1);
            TextureFile file;
            file.setFile(image);
            auto result = file.saveToDisk(filePath);
            CHECK(result == 0);
        }
        SUBCASE("Loading file from disk")
        {
            TextureFile file;
            auto result = file.loadFromDisk(filePath);
            CHECK(result == 0);
            const auto &loadedImage = file.getFile();
            CHECK(loadedImage.GetMetadata().width == 16);
            CHECK(loadedImage.GetMetadata().height == 16);
            CHECK(file.isTGA() == false);
            QFile::remove(filePath);
        }
    }
}
