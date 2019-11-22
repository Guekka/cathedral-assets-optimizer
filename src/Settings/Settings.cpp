/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Settings.hpp"
#include "Utils/InitJSON.hpp"

namespace CAO {

Settings::Settings()
{
    initJSON(_json);
}

void Settings::saveToJSON(const QString &filepath) const
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::out);
    if (!stream.is_open())
        return;

    stream << _json.dump(4);
}

void Settings::readFromJSON(const QString &filepath)
{
    std::fstream stream(std::filesystem::u8path(filepath.toStdString()), std::fstream::in);
    if (!stream.is_open())
        return;

    stream >> _json;
}

#ifdef GUI
void Settings::readFromUi(Ui::MainWindow *ui)
{
    AdvancedSettings::readFromUi(ui, _json);
    StandardSettings::readFromUi(ui, _json);
}

void Settings::saveToUi(Ui::MainWindow *ui)
{
    AdvancedSettings::saveToUi(ui, _json);
    StandardSettings::saveToUi(ui, _json);
}
#endif

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
    StandardSettings::OptimizationMode mode;
    if (readMode == "om")
        mode = StandardSettings::SingleMod;
    else if (readMode == "sm")
        mode = StandardSettings::SeveralMods;
    else
        throw std::runtime_error("Invalid argument for mode");

    const QString &readGame = parser.positionalArguments().at(2);
    Profiles::setCurrentProfile(readGame);

    //BSA
    setValue(StandardSettings::bBsaExtract, parser.isSet("be"));
    setValue(StandardSettings::bBsaCreate, parser.isSet("bc"));
    setValue(StandardSettings::bBsaDeleteBackup, parser.isSet("bd"));

    //Textures
    setValue(StandardSettings::bTexturesNecessary, parser.isSet("t0"));
    setValue(StandardSettings::bTexturesCompress, parser.isSet("t1"));
    setValue(StandardSettings::bTexturesMipmaps, parser.isSet("t2"));

    //Textures resizing
    setValue(StandardSettings::bTexturesResizeSize, parser.isSet("trs"));
    setValue(StandardSettings::iTexturesTargetWidth, parser.value("trsw").toUInt());
    setValue(StandardSettings::iTexturesTargetHeight, parser.value("trsh").toUInt());

    setValue(StandardSettings::bTexturesResizeRatio, parser.value("trr").toUInt());
    setValue(StandardSettings::iTexturesTargetWidthRatio, parser.value("trrw").toUInt());
    setValue(StandardSettings::iTexturesTargetHeightRatio, parser.value("trrh").toUInt());

    //Meshes
    setValue(StandardSettings::iMeshesOptimizationLevel, parser.value("m").toInt());
    setValue(StandardSettings::bMeshesHeadparts, parser.isSet("mh"));
    setValue(StandardSettings::bMeshesResave, parser.isSet("mr"));

    //Animations
    setValue(StandardSettings::bAnimationsOptimization, parser.isSet("a"));

    //General
    setValue(StandardSettings::bDryRun, parser.isSet("dr"));
    setValue(StandardSettings::sUserPath, path);
    setValue(StandardSettings::eMode, mode);
    setValue(StandardSettings::bDebugLog, parser.isSet("l"));
}

const json &Settings::splitKey(const QString &key) const
{
    auto split = key.split("/");
    const json *j = &_json;
    for (const auto &subKey : split)
        j = &(*j)[subKey.toStdString()];

    return *j;
}

json &Settings::splitKey(const QString &key)
{
    return const_cast<json &>(const_cast<const Settings *>(this)->splitKey(key));
}

const json &Settings::splitKeySafe(const QString &key) const
{
    auto split = key.split("/");
    const json *j = &_json;
    for (const auto &subKey : split)
        j = &j->at(subKey.toStdString());

    return *j;
}

json &Settings::splitKeySafe(const QString &key)
{
    return const_cast<json &>(const_cast<const Settings *>(this)->splitKeySafe(key));
}

QString Settings::isValid() const
{
    if (!QDir(getMandatoryValue<QString>(StandardSettings::sUserPath)).exists()
        || getMandatoryValue<QString>(StandardSettings::sUserPath).size() < 5)
    {
        return QString("This path does not exist or is shorter than 5 characters. Path: '%1'")
            .arg(getMandatoryValue<QString>(StandardSettings::sUserPath));
    }

    if (StandardSettings::iMeshesOptimizationLevel < 0 || StandardSettings::iMeshesOptimizationLevel > 3)
        return ("This meshes optimization level does not exist. Level: "
                + QString::number(StandardSettings::iMeshesOptimizationLevel));

    if (getMandatoryValue<uint>(StandardSettings::iTexturesTargetWidth) % 2 != 0
        || getMandatoryValue<uint>(StandardSettings::iTexturesTargetHeight) % 2 != 0)
        return ("Textures target size has to be a power of two");

    return QString();
}
} // namespace CAO
