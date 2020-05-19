/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "doctest/doctest.h"

#include "File/Textures/TextureFile.hpp"
#include "Settings/Profiles.hpp"

static void initSettings()
{
    const QString &profileName = "TestingProfile";
    CAO::Profiles::getInstance().create(profileName);
    auto &profile = CAO::Profiles::getInstance().setCurrent(profileName);

    //Resetting
    profile.getGeneralSettings() = {};
    profile.getPatterns().get()  = {{0, CAO::PatternSettings{}}};

    profile.saveToJSON();
}

static std::unique_ptr<CAO::TextureFile> getStandardTextureFile(
    const bool optimizedFile = true, const DXGI_FORMAT &format = DXGI_FORMAT_A8_UNORM)
{
    auto textureResource = new CAO::TextureResource;
    auto file            = std::make_unique<CAO::TextureFile>();
    textureResource->Initialize2D(format, 16, 16, 1, 1);
    textureResource->origFormat = format;
    file->setFile(*textureResource, optimizedFile);
    file->setName("TextureTest");

    //Checking that the file is valid
    auto res = dynamic_cast<const CAO::TextureResource *>(&file->getFile());
    REQUIRE(res->GetImages());

    return file;
}

static CAO::PatternSettings &getPatternSettings()
{
    return CAO::currentProfile().getPatterns().getDefaultSettings();
}
