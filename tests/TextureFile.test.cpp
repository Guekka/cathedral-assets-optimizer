#include "utils.hpp"

using namespace CAO;

SCENARIO("Saving and loading a dds file to the disk")
{
    GIVEN("A valid TextureFile and a valid path")
    {
        auto file = getStandardTextureFile(PatternSettings{}, false);
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
                    REQUIRE(file2.loadFromDisk(filePath) == 0);
                    const auto info1 = dynamic_cast<const TextureResource *>(&file->getFile())->GetMetadata();
                    const auto info2 = dynamic_cast<const TextureResource *>(&file2.getFile())->GetMetadata();
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
