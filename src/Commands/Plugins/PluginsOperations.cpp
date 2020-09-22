/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/Plugins/PluginsOperations.hpp"
#include "Settings/Games.hpp"
#include "Settings/Profiles.hpp"
#include "Utils/Algorithms.hpp"
#include "Utils/Filesystem.hpp"

namespace CAO {
void PluginsOperations::makeDummyPlugins(const QString &folderPath, const GeneralSettings &settings)
{
    const auto &game = GameSettings::get(settings.eGame());

    if (!game.sDummyPlugin().has_value())
        return;

    PLOG_VERBOSE << "Creating enough dummy plugins to load BSAs";

    QDir dir(folderPath);
    for (const auto &bsaName : listBSAsNames(QDirIterator(folderPath, QDirIterator::Subdirectories), game))
    {
        if (checkIfBsaHasPlugin(dir.absoluteFilePath(bsaName), game))
            continue;

        const std::filesystem::path dummyPath = QString(folderPath + "/" + bsaName + ".esp").toStdString();
        std::ofstream dummy(dummyPath, std::ios::out | std::ios::binary);

        const auto dummyBytes = game.sDummyPlugin().value();
        dummy.write(reinterpret_cast<const char *>(dummyBytes.data()), dummyBytes.size());
    }
}

QString PluginsOperations::findPlugin(const QDir &folderPath, const GameSettings &settings)
{
    const auto &[bsaSuffix, bsaTexSuffix] = getBSASuffixes(settings);

    QStringList espNames;
    QDirIterator it(folderPath);
    while (it.hasNext())
    {
        it.next();
        const QString espName = it.fileName().remove(QRegularExpression("\\.es[lmp]"));
        if (espName == it.fileName() || espName.isEmpty())
            continue;
        espNames << espName;
    }
    espNames.removeDuplicates();

    if (espNames.isEmpty())
        espNames << folderPath.dirName();

    for (size_t i = 0; i < 1000; ++i)
    {
        for (const auto &esp : espNames)
        {
            const QString &texBsaName = folderPath.filePath(esp + bsaTexSuffix);
            const QString &bsaName    = folderPath.filePath(esp + bsaSuffix);
            const bool goodName       = !QFile(texBsaName).exists() && !QFile(bsaName).exists();

            if (goodName)
                return esp;
        }
        const QString &newEspName = espNames.first() + QString::number(i);
        espNames.clear();
        espNames << newEspName;
    }
    throw std::runtime_error("No plugin name found after 1 000 tries.");
}

bool PluginsOperations::checkIfBsaHasPlugin(const QString &bsaPath, const GameSettings &settings)
{
    QDir bsaDir     = QFileInfo(bsaPath).absoluteDir();
    QString bsaName = QFileInfo(bsaPath).fileName();

    const auto &[bsaSuffix, bsaTexSuffix] = getBSASuffixes(settings);

    bsaName.remove(bsaSuffix);
    bsaName.remove(bsaTexSuffix);

    const QStringList pluginNames = {bsaName + ".esl", bsaName + ".esm", bsaName + ".esp"};
    for (const auto &name : pluginNames)
        if (bsaDir.exists(name))
            return true;

    return false;
}

QStringList PluginsOperations::listBSAsNames(QDirIterator it, const GameSettings &settings)
{
    const auto &[bsaSuffix, bsaTexSuffix] = getBSASuffixes(settings);

    QStringList bsas;
    while (it.hasNext())
    {
        it.next();
        QString bsaName = it.fileName();
        if (bsaName.endsWith(bsaSuffix, Qt::CaseInsensitive)
            || bsaName.endsWith(bsaTexSuffix, Qt::CaseInsensitive))
        {
            bsas.push_back(bsaName.remove(bsaSuffix).remove(bsaTexSuffix));
        }
    }
    bsas.removeDuplicates();
    return bsas;
}

std::vector<std::string> PluginsOperations::listHeadparts(const QString &filepath)
{
    auto file = Filesystem::openBinaryFile(filepath);

    if (!file.is_open())
        return {};

    PluginRecordHeader header{};
    PluginFieldHeader pluginFieldHeader{};

    std::vector<std::string> headparts;
    headparts.reserve(1000);

    const auto readHeaders = [&]() { file.read(reinterpret_cast<char *>(&header), sizeof header); };

    const auto readFieldPluginHeader = [&]() {
        file.read(reinterpret_cast<char *>(&pluginFieldHeader), sizeof pluginFieldHeader);
    };

    readHeaders();
    if (strncmp(header.plugin.type, GROUP_TES4, sizeof GROUP_TES4) != 0)
        return {}; //Not a plugin file

    //Skip TES4 record
    file.seekg(header.record.dataSize, std::ios::cur);

    //Reading all groups
    do
    {
        readHeaders();

        // skip non headpart groups
        if (strncmp(header.plugin.label, GROUP_HDPT, sizeof GROUP_HDPT) != 0)
        {
            file.seekg(header.plugin.groupSize - sizeof header, std::ios::cur);
            continue;
        }

        //Reading all headpart records
        const int64_t groupEndPos = header.plugin.groupSize - sizeof header + file.tellg();
        while (file.tellg() < groupEndPos)
        {
            readHeaders();

            // reading all record fields
            const int64_t recEndPos = header.record.dataSize + file.tellg();
            while (file.tellg() < recEndPos)
            {
                readFieldPluginHeader();
                // skip everything but MODL
                if (strncmp(pluginFieldHeader.type, GROUP_MODL, sizeof GROUP_MODL) != 0)
                {
                    file.seekg(pluginFieldHeader.dataSize, std::ios::cur);
                    continue;
                }

                char buffer[1024];
                assert(pluginFieldHeader.dataSize < 1024);
                file.read(buffer, pluginFieldHeader.dataSize);

                // make sure that nif path starts with meshes
                QString headpart     = std::move(buffer);
                const QString prefix = headpart.startsWith("meshes", Qt::CaseInsensitive) ? "*/"
                                                                                          : "*/meshes/";
                headpart.prepend(prefix);

                //Adding headparts to the list
                headparts.emplace_back(QDir::cleanPath(headpart).toStdString());
            }
        }
    } while (strncmp(header.plugin.type, GROUP_GRUP, sizeof GROUP_GRUP) == 0 && file.good());

    return headparts;
}

std::vector<std::string> PluginsOperations::listLandscapeTextures(const QString &filepath)
{
    auto file = Filesystem::openBinaryFile(filepath);

    if (!file.is_open())
        return {};

    PluginRecordHeader header{};
    PluginFieldHeader pluginFieldHeader{};

    const auto readHeaders = [&]() {
        file.read(reinterpret_cast<char *>(&header), sizeof header);
        return strncmp(header.plugin.type, GROUP_GRUP, sizeof GROUP_GRUP) == 0;
    };

    const auto readFieldPluginHeader = [&]() {
        file.read(reinterpret_cast<char *>(&pluginFieldHeader), sizeof pluginFieldHeader);
    };

    readHeaders();
    if (strncmp(header.plugin.type, GROUP_TES4, sizeof GROUP_TES4) != 0)
        return {}; //Not a plugin file

    //Skip TES4 record
    file.seekg(header.record.dataSize, std::ios::cur);

    std::vector<uint32_t> firstSet;
    std::map<uint32_t, std::string> slTextures;
    std::vector<std::string> finalTextures;

    //Reading all groups
    while (readHeaders() && file.good())
    {
        // skip other groups
        if (strncmp(header.plugin.label, GROUP_LTEX, sizeof GROUP_LTEX) != 0
            && strncmp(header.plugin.label, GROUP_TXST, sizeof GROUP_TXST) != 0)
        {
            file.seekg(header.plugin.groupSize - sizeof header, std::ios::cur);
            continue;
        }

        char signatureGroup[4];
        memcpy(signatureGroup, header.plugin.label, 4);

        //Reading all records
        const int64_t groupEndPos = header.plugin.groupSize - sizeof header + file.tellg();
        while (file.tellg() < groupEndPos)
        {
            readHeaders();

            // reading all record fields
            const int64_t recEndPos = header.record.dataSize + file.tellg();
            while (file.tellg() < recEndPos)
            {
                readFieldPluginHeader();
                // read FormID of landscape TXST record
                if (strncmp(signatureGroup, GROUP_LTEX, sizeof GROUP_LTEX) == 0
                    && strncmp(pluginFieldHeader.type, GROUP_TNAM, sizeof GROUP_TNAM) == 0)
                {
                    uint32_t formId = 0;
                    file.read(reinterpret_cast<char *>(&formId), pluginFieldHeader.dataSize);
                    firstSet.push_back(formId);
                }
                // read diffuse texture name from TXST record
                else if (strncmp(signatureGroup, GROUP_TXST, sizeof GROUP_TXST) == 0
                         && strncmp(pluginFieldHeader.type, GROUP_TX00, sizeof GROUP_TX00) == 0)
                {
                    char buffer[1024];
                    assert(pluginFieldHeader.dataSize < 1024);
                    file.read(buffer, pluginFieldHeader.dataSize);

                    QString string       = QDir::cleanPath(std::move(buffer));
                    const QString prefix = string.startsWith("textures", Qt::CaseInsensitive) ? "*/"
                                                                                              : "*/textures/";
                    string.prepend(prefix);

                    slTextures.try_emplace(header.record.id, string.toStdString());
                }

                //Skip other fields
                else
                    file.seekg(pluginFieldHeader.dataSize, std::ios::cur);
            }
        }
    }

    // go over landscape texture set FormIDs and find matching diffuse textures
    for (const auto &id : firstSet)
    {
        auto it = slTextures.find(id);
        if (it != slTextures.end())
            finalTextures.emplace_back(it->second);
    }

    return finalTextures;
}

std::pair<QString, QString> PluginsOperations::getBSASuffixes(const GameSettings &sets)
{
    const auto &bsaSuffix = sets.sBSASuffix();
    //Doesn't matter if they're the same
    const auto &bsaTexSuffix = sets.sBSATexturesSuffix().value_or(bsaSuffix);

    return {bsaSuffix, bsaTexSuffix};
}

} // namespace CAO
