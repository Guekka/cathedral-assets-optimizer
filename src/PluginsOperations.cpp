/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PluginsOperations.h"

void PluginsOperations::makeDummyPlugins(const QString &folderPath)
{
    PLOG_VERBOSE << "Creating enough dummy plugins to load BSAs";

    QDirIterator it(folderPath);

    while (it.hasNext())
    {
        QString espName;

        it.next();

        if (!checkIfBsaHasPlugin(it.filePath()) && it.fileName().endsWith(Profiles::bsaExtension(), Qt::CaseInsensitive))
        {
            if (it.fileName().contains(Profiles::bsaTexturesSuffix(), Qt::CaseInsensitive))
            {
                espName = it.fileName().remove(Profiles::bsaTexturesSuffix(), Qt::CaseInsensitive) + ".esp";
                PLOG_VERBOSE << "Created textures bsa plugin:" + espName;
            }

            else
            {
                espName = it.fileName().remove(Profiles::bsaSuffix(), Qt::CaseInsensitive) + ".esp";
                PLOG_VERBOSE << "Created standard bsa plugin:" + espName;
            }
            Profiles::getFile("DummyPlugin.esp").copy(folderPath + "/" + espName);
        }
    }
}

QString PluginsOperations::findPlugin(const QString &folderPath, const BsaType &bsaType)
{
    QDirIterator it(folderPath);
    QStringList espName;
    QString bsaName;

    while (it.hasNext())
    {
        it.next();

        if (it.fileName().contains(QRegularExpression("\\.es[plm]$")))
            espName << it.fileName();

        if (it.fileName().endsWith(Profiles::bsaSuffix(), Qt::CaseInsensitive)
            && !it.fileName().endsWith(Profiles::bsaTexturesSuffix(), Qt::CaseInsensitive))
        {
            bsaName = it.fileName().chopped(4) + ".esp";
        }
    }
    if (!bsaName.isEmpty() && espName.isEmpty())
        espName << bsaName;

    if (espName.isEmpty())
        espName << QDir(folderPath).dirName() + ".esp";

    QString returnedEsp;
    int counter = 0;

    do
    {
        for (const auto &esp : espName)
        {
            const bool texturesBsaGood = !QFile(folderPath + "/" + esp.chopped(4) + Profiles::bsaTexturesSuffix()).exists()
                                         && bsaType == TexturesBsa;

            const bool standardBsaGood = !QFile(folderPath + "/" + esp.chopped(4) + Profiles::bsaSuffix()).exists()
                                         && bsaType != TexturesBsa;

            if (texturesBsaGood || standardBsaGood)
                returnedEsp = esp;
        }
        if (returnedEsp.isEmpty())
            espName << espName.last().chopped(4) + QString::number(counter) + ".esp";

        ++counter;
    } while (returnedEsp.isEmpty());

    return returnedEsp.remove(QRegularExpression("\\.es[plm]$"));
}

bool PluginsOperations::checkIfBsaHasPlugin(const QString &bsaPath)
{
    QString bsaName = QFileInfo(bsaPath).fileName();
    bsaName.remove(Profiles::bsaSuffix());
    bsaName.remove(Profiles::bsaTexturesSuffix());

    const QStringList pluginNames = {bsaName + ".esl", bsaName + ".esm", bsaName + ".esp"};
    for (const auto &name : pluginNames)
        if (QFile(name).exists())
            return true;

    return false;
}

QStringList PluginsOperations::listHeadparts(const QString &filepath)
{
    std::fstream file;
    file.open(filepath.toStdString(), std::ios::binary | std::ios::in);

    if (!file.is_open())
        return QStringList();

    PluginRecordHeader header{};
    PluginFieldHeader pluginFieldHeader{};

    QStringList headparts;

    const auto readHeaders = [&]() { file.read(reinterpret_cast<char *>(&header), sizeof header); };

    const auto readFieldPluginHeader = [&]() {
        file.read(reinterpret_cast<char *>(&pluginFieldHeader), sizeof pluginFieldHeader);
    };

    readHeaders();
    if (strncmp(header.plugin.type, GROUP_TES4, sizeof GROUP_TES4) != 0)
        return QStringList(); //Not a plugin file

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
                file.read(buffer, pluginFieldHeader.dataSize);

                QString headpart = buffer;
                // make sure that nif path starts with meshes
                if (!headpart.startsWith("meshes", Qt::CaseInsensitive))
                    headpart = "meshes/" + headpart;

                //Adding headparts to the list
                headparts << QDir::cleanPath(headpart);
            }
        }
    } while (strncmp(header.plugin.type, GROUP_GRUP, sizeof GROUP_GRUP) == 0 && file.good());

    return headparts;
}

QStringList PluginsOperations::listLandscapeTextures(const QString &filepath)
{
    std::fstream file;
    file.open(filepath.toStdString(), std::ios::binary | std::ios::in);

    if (!file.is_open())
        return QStringList();

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
        return QStringList(); //Not a plugin file

    //Skip TES4 record
    file.seekg(header.record.dataSize, std::ios::cur);

    std::vector<uint32_t> firstSet;
    QMap<uint32_t, QString> slTextures;
    QStringList finalTextures;

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
                    file.read(buffer, pluginFieldHeader.dataSize);
                    QString string = QDir::cleanPath(buffer);
                    if (!string.startsWith("textures/"))
                        string.insert(0, "textures/");

                    slTextures.insert(header.record.id, string);
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
        const auto &idx = slTextures.value(id);
        if (!idx.isEmpty())
            finalTextures << idx;
    }

    return finalTextures;
}
