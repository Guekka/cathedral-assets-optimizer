/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "doctest/doctest.h"

#include "File/Textures/TextureFile.hpp"
#include "Settings/Profiles.hpp"

static void setSettings(const CAO::PatternSettings &pSets, const CAO::GeneralSettings &gSets)
{
    CAO::currentProfile().getPatterns().get().clear();
    CAO::currentProfile().getPatterns().addPattern(pSets);
    CAO::currentProfile().getGeneralSettings() = gSets;
    CAO::currentProfile().saveToJSON();
}

static void initSettings()
{
    const QString &profileName = "TestingProfile";
    CAO::getProfiles().create(profileName);
    CAO::getProfiles().setCurrent(profileName);

    //Resetting
    setSettings(CAO::PatternSettings{}, CAO::GeneralSettings{});
}

static std::unique_ptr<CAO::TextureFile> getStandardTextureFile(
    const CAO::PatternSettings &pSets,
    const bool optimizedFile          = true,
    const DXGI_FORMAT &format         = DXGI_FORMAT_A8_UNORM,
    const CAO::GeneralSettings &gSets = CAO::GeneralSettings{})
{
    setSettings(pSets, gSets);

    auto textureResource = std::make_unique<CAO::TextureResource>();
    textureResource->Initialize2D(format, 16, 16, 1, 1);
    textureResource->origFormat = format;

    //Assigning an orig format different than the current one. This would also happen
    //if the file was really optimized
    if (optimizedFile)
        textureResource->origFormat = format == DXGI_FORMAT_BC7_UNORM ? DXGI_FORMAT_A8_UNORM
                                                                      : DXGI_FORMAT_BC7_UNORM;

    auto file = std::make_unique<CAO::TextureFile>();
    file->setFile(std::unique_ptr<CAO::Resource>(std::move(textureResource)), optimizedFile);
    file->setInputFilePath("TextureTest");

    //Checking that the file is valid
    auto res = dynamic_cast<const CAO::TextureResource *>(&file->getFile());
    REQUIRE(res);
    REQUIRE(res->GetImages());

    return file;
}

namespace doctest {
template<typename T>
struct StringMaker<std::vector<T>>
{
    static String convert(const std::vector<T> &in)
    {
        std::ostringstream oss;

        oss << "[\n";
        for (const auto &val : in)
            oss << val << ", ";
        oss << "\n]";

        return oss.str().c_str();
    }
};

template<>
struct StringMaker<std::vector<CAO::PatternSettings>>
{
    static String convert(const std::vector<CAO::PatternSettings> &in)
    {
        std::ostringstream oss;

        oss << "[\n";
        for (const auto &val : in)
            oss << val.getJSON().dump(4) << "\n;\n";
        oss << "\n]";

        return oss.str().c_str();
    }
};

template<>
struct StringMaker<nlohmann::json>
{
    static String convert(const nlohmann::json &in)
    {
        std::ostringstream oss;

        oss << in.dump(4);

        return oss.str().c_str();
    }
};
} // namespace doctest
