/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "File/Textures/TextureFile.hpp"
#include "settings/settings.hpp"
#include "doctest/doctest.h"

static void setSettings(const cao::PerFileSettings &pSets, const cao::GeneralSettings &gSets)
{
    cao::currentProfile().getPatterns().get().clear();
    cao::currentProfile().getPatterns().addPattern(pSets);
    cao::currentProfile().getGeneralSettings() = gSets;
    cao::currentProfile().saveToJSON();
}

static void initSettings()
{
    const QString &profileName = "TestingProfile";
    cao::getProfiles().create_profile(profileName);
    cao::getProfiles().setCurrent(profileName);

    //Resetting
    setSettings(cao::PerFileSettings{}, cao::GeneralSettings{});
}

static std::unique_ptr<cao::TextureFile> getStandardTextureFile(
    const cao::PerFileSettings &pSets,
    const bool optimizedFile          = true,
    const DXGI_FORMAT &format         = DXGI_FORMAT_A8_UNORM,
    const cao::GeneralSettings &gSets = cao::GeneralSettings{})
{
    setSettings(pSets, gSets);

    cao::Resources::Texture textureResource;
    textureResource.Initialize2D(format, 16, 16, 1, 1);
    textureResource.origFormat = format;

    //Assigning an orig format different than the current one. This would also happen
    //if the file was really optimized
    if (optimizedFile)
        textureResource.origFormat = format == DXGI_FORMAT_BC7_UNORM ? DXGI_FORMAT_A8_UNORM
                                                                     : DXGI_FORMAT_BC7_UNORM;

    auto file = std::make_unique<cao::TextureFile>();
    file->setFile(std::move(textureResource), optimizedFile);
    file->setInputFilePath("TextureTest");

    //Checking that the file is valid
    const auto *res = file->getFile<cao::Resources::Texture>();
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
struct StringMaker<std::vector<cao::PerFileSettings>>
{
    static String convert(const std::vector<cao::PerFileSettings> &in)
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
