/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License= v. 2.0. If a copy of the MPL was not distributed with this
 * file= You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QDirIterator>
#include <QSettings>

#include "Settings/BaseTypes.hpp"
#include "Settings/Profiles.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {

bool profileVersionIs5(const QDir &profile)
{
    return profile.exists("profile.ini");
}

Games guessGame(bsa_archive_type_t bsaFormat)
{
    switch (bsaFormat)
    {
        case baTES3: return Games::Morrowind;
        case baTES4: return Games::Oblivion;
        case baFO3: return Games::SkyrimLE; //Could also be FO3 and FNV, SSE is the most common
        case baFO4:
        case baFO4dds: return Games::Fallout4;
        case baSSE:
        default: return Games::SkyrimSE;
    }
}

void migrate5To6(const QDir &oldProfile, Profile &outProfile)
{
    const QString inFile = oldProfile.absoluteFilePath("profile.ini");
    QSettings profileSettings(inFile, QSettings::IniFormat);

    auto gPattern     = PatternSettings{0, {"*"}};
    auto &generalSets = outProfile.getGeneralSettings();

    profileSettings.beginGroup("BSA");

    generalSets.eGame = guessGame(static_cast<bsa_archive_type_t>(profileSettings.value("bsaFormat").toInt()));

    generalSets.iBSAMaxSize = profileSettings.value("maxBsaUncompressedSize").toDouble();

    generalSets.bBSATexturesEnabled = profileSettings.value("hasBsaTextures").toBool();
    generalSets.iBSATexturesMaxSize = profileSettings.value("maxBsaTexturesSize").toDouble();

    profileSettings.endGroup();
    profileSettings.beginGroup("Textures");

    gPattern.eTexturesFormat       = profileSettings.value("texturesFormat").value<DXGI_FORMAT>();
    bool texturesConvertTga        = profileSettings.value("texturesConvertTga").toBool();
    auto texturesUnwantedFormats   = profileSettings.value("texturesUnwantedFormats").toList();
    bool texturesCompressInterface = profileSettings.value("texturesCompressInterface").toBool();

    profileSettings.endGroup();

    PatternSettings pSetsTGA{1, {"*.tga"}};
    pSetsTGA.bTexturesForceConvert = texturesConvertTga;

    PatternSettings pSetsInterfaceDDS{2, "*/interface/*.dds"};
    pSetsInterfaceDDS.bTexturesForceConvert = texturesCompressInterface;

    PatternSettings pSetsInterfaceTGA{3, "*/interface/*.tga"};
    pSetsInterfaceTGA.bTexturesForceConvert = texturesCompressInterface;

    PatternSettings pSetsLodgen{4, {"*/textures/terrain/lodgen/*"}};
    pSetsLodgen.bTexturesMipmaps        = false;
    pSetsLodgen.bTexturesCompress       = false;
    pSetsLodgen.bTexturesForceConvert   = false;
    pSetsLodgen.bTexturesLandscapeAlpha = false;

    std::vector<DXGI_FORMAT> unwantedFormats;
    auto getFormat                    = [](const QVariant &variant) { return variant.value<DXGI_FORMAT>(); };
    gPattern.slTextureUnwantedFormats = texturesUnwantedFormats | rx::transform(getFormat) | rx::to_vector();

    PatternMap &patterns = outProfile.getPatterns();
    patterns.addPattern(gPattern);
    patterns.addPattern(pSetsTGA);
    patterns.addPattern(pSetsInterfaceDDS);
    patterns.addPattern(pSetsInterfaceTGA);
    patterns.addPattern(pSetsLodgen);
}

void convertFiles5To6(const QDir &oldProfile, Profile &outProfile)
{
    auto readFile = [](const QString &path) {
        QFile file(path);
        auto list = Filesystem::readFile(file, [](QString &line) { return line.prepend('*').append('*'); });
        return std::move(list) | rx::transform([](QString val) { return val.toStdString(); })
               | rx::to_vector();
    };

    QDirIterator it(oldProfile);
    while (it.hasNext())
    {
        it.next();
        const QString &filename = it.fileName();

        if (filename == "customHeadparts.txt")
            outProfile.getFileTypes().slMeshesHeadparts = readFile(it.filePath());

        else if (filename == "customLandscape.txt")
            outProfile.getFileTypes().slTextureLandscapes = readFile(it.filePath());

        else if (filename == "FilesToNotPack.txt")
            outProfile.getFileTypes().slBSABlacklist = readFile(it.filePath());

        else if (filename == "ignoredMods.txt")
            outProfile.getFileTypes().slModsBlacklist = readFile(it.filePath());

        else if (filename == "isBase")
            outProfile.getGeneralSettings().isBaseProfile = true;
    }
}

void addDefaultValues5To6(Profile &outProfile)
{
    auto &ft = outProfile.getFileTypes();

    if (ft.slBSAStandardFiles().empty())
    {
        ft.slBSAStandardFiles = {"*.bgem", "*.bgsm", "*.bto", "*.btr", "*.btt", "*.dlodsettings", "*.dtl",
                                 "*.egm",  "*.gid",  "*.hkx", "*.lod", "*.lst", "*.nif",          "*.pex",
                                 "*.png",  "*.psc",  "*.seq", "*.swf", "*.tri", "*.txt"};
    }

    if (ft.slBSATextureFiles().empty())
    {
        ft.slBSATextureFiles = {"*.dds"};
    }

    if (ft.slBSAUncompressibleFiles().empty())
    {
        ft.slBSAUncompressibleFiles = {"*.fuz", "*.lip", "*.ogg", "*.wav", "*.xwm", "*.*script"};
    }

    ft.slModsBlacklist.insert(std::string("*_separator"), false);
}

QStringList migrateProfiles(const QDir &oldProfileRoot, const QDir &newProfileRoot)
{
    Profiles profiles(newProfileRoot);
    QStringList migratedProfiles;
    for (const auto &dir : oldProfileRoot.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        try
        {
            const QString &profileName = dir + "_migrated";
            QDir oldDir{oldProfileRoot.absoluteFilePath(dir)};
            QDir newDir{newProfileRoot.absoluteFilePath(profileName)};

            if (!profileVersionIs5(oldDir))
                continue;

            profiles.create(profileName);
            auto &profile = profiles.get(profileName);
            migrate5To6(oldDir, profile);
            convertFiles5To6(oldDir, profile);
            addDefaultValues5To6(profile);
            profile.saveToJSON();

            oldDir.removeRecursively();

            migratedProfiles.push_back(oldDir.dirName());
        }
        catch (const std::exception &)
        {
            continue;
        }
    }
    return migratedProfiles;
}
} // namespace CAO
