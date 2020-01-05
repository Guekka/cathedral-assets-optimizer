/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Settings/Profiles.hpp"

namespace CAO {

Settings::Settings(nlohmann::json j)
    : _json(j)
{
}

void Settings::parseArguments(const QStringList &args)
{
    if (args.count() < 4)
        throw std::runtime_error("Not enough arguments");
    QCommandLineParser parser;

    parser.addHelpOption();

    parser.addPositionalArgument("folder", "The folder to process, surrounded with quotes.");
    parser.addPositionalArgument("mode", "Either om (one mod) or sm (several mods)");
    parser.addPositionalArgument("profile", "One of the profile located in CAO/profiles");

    parser.addOptions({
        {"dr", "Enables dry run"},
        {"l", "Enables debug log"},
        {"m",
         "Mesh processing level: 0 (default) to disable optimization, 1 for necessary optimization, "
         "2 for medium optimization, 3 for full optimization.",
         "value",
         "0"},

        {"t0", "Enables textures necessary optimization"},
        {"t1", "Enables textures compression"},
        {"t2", "Enables textures mipmaps generation"},

        {"trr", "Enables textures resizing by ratio"},
        {"trrw", "The width ratio"},
        {"trrh", "The height ratio"},

        {"trs", "Enables textures resizing by fixed size"},
        {"trsw", "The width size"},
        {"trsh", "The height size"},

        {"a", "Enables animations processing"},
        {"mh", "Enables headparts detection and processing"},
        {"mr", "Enables meshes resaving"},
        {"be", "Enables BSA extraction."},
        {"bc", "Enables BSA creation."},
        {"bd", "Enables deletion of BSA backups."},
        /*{"bo",
         "NOT WORKING. Enables BSA optimization. The files inside the "
         "BSA will be extracted to memory and processed according to the provided settings "},*/
    });

    parser.process(args);
    const QString &path = QDir::cleanPath(parser.positionalArguments().at(0));
    const QString readMode = parser.positionalArguments().at(1);
    OptimizationMode mode;
    if (readMode == "om")
        mode = SingleMod;
    else if (readMode == "sm")
        mode = SeveralMods;
    else
        throw std::runtime_error("Invalid argument for mode");

    const QString &readGame = parser.positionalArguments().at(2);
    Profiles().setCurrentProfile(readGame);

    //BSA
    setValue(bBsaExtract, parser.isSet("be"));
    setValue(bBsaCreate, parser.isSet("bc"));
    setValue(bBsaDeleteBackup, parser.isSet("bd"));

    //Textures
    setValue(bTexturesNecessary, parser.isSet("t0"));
    setValue(bTexturesCompress, parser.isSet("t1"));
    setValue(bTexturesMipmaps, parser.isSet("t2"));

    //Textures resizing
    setValue(bTexturesResizeSize, parser.isSet("trs"));
    setValue(iTexturesTargetWidth, parser.value("trsw").toUInt());
    setValue(iTexturesTargetHeight, parser.value("trsh").toUInt());

    setValue(bTexturesResizeRatio, parser.value("trr").toUInt());
    setValue(iTexturesTargetWidthRatio, parser.value("trrw").toUInt());
    setValue(iTexturesTargetHeightRatio, parser.value("trrh").toUInt());

    //Meshes
    setValue(iMeshesOptimizationLevel, parser.value("m").toInt());
    setValue(bMeshesHeadparts, parser.isSet("mh"));
    setValue(bMeshesResave, parser.isSet("mr"));

    //Animations
    setValue(bAnimationsOptimization, parser.isSet("a"));

    //General
    setValue(bDryRun, parser.isSet("dr"));
    setValue(sUserPath, path);
    setValue(eMode, mode);
    setValue(bDebugLog, parser.isSet("l"));
}

QString Settings::isValid() const
{
    if (!QDir(getValue<QString>(sUserPath)).exists() || getValue<QString>(sUserPath).size() < 5)
    {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(getValue<QString>(sUserPath));
    }

    const auto &meshOptLevel = getValue<uint>(iMeshesOptimizationLevel);
    if (meshOptLevel > 3)
        return ("This meshes optimization level does not exist. Level: " + QString::number(meshOptLevel));

    if (getValue<uint>(iTexturesTargetWidth) % 2 != 0 || getValue<uint>(iTexturesTargetHeight) % 2 != 0)
        return ("Textures target size has to be a power of two");

    return QString();
}
} // namespace CAO
