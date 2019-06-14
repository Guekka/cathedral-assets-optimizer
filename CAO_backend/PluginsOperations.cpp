/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "PluginsOperations.h"

void PluginsOperations::makeDummyPlugins(const QString& folderPath)
{
    QLogger::QLog_Trace("PluginsOperations", "Entering makeDummyPluginsfunction: creating enough dummy plugins to load BSAs");

    QDirIterator it(folderPath);

    while (it.hasNext())
    {
        QString espName;

        it.next();

        if(!checkIfBsaHasPlugin(it.filePath()) && it.fileName().endsWith(CAO_BSA_EXTENSION, Qt::CaseInsensitive))
        {
            if(it.fileName().contains(CAO_BSA_TEXTURES_SUFFIX, Qt::CaseInsensitive))
            {
                espName = it.fileName().remove(CAO_BSA_TEXTURES_SUFFIX + CAO_BSA_EXTENSION, Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created textures bsa plugin:" + espName);
            }

            else
            {
                espName = it.fileName().remove(CAO_BSA_SUFFIX + CAO_BSA_EXTENSION, Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created standard bsa plugin:" + espName);
            }
            QFile::copy(CAO_RESOURCES_PATH + "DummyPlugin.esp", folderPath + "/" + espName);
        }
    }
    QLogger::QLog_Trace("PluginsOperations", "Exiting makeDummyPlugins function");
}


QString PluginsOperations::findPlugin(const QString& folderPath, const bsaType& bsaType)
{
    QDirIterator it(folderPath);
    QStringList espName;
    QString bsaName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]$")))
            espName << it.fileName();


        if(it.fileName().endsWith(CAO_BSA_SUFFIX + CAO_BSA_EXTENSION, Qt::CaseInsensitive) &&
                !it.fileName().endsWith(CAO_BSA_TEXTURES_SUFFIX + CAO_BSA_EXTENSION, Qt::CaseInsensitive))
        {
            bsaName = it.fileName().chopped(4) + ".esp";
        }
    }
    if(!bsaName.isEmpty() && espName.isEmpty())
        espName << bsaName;

    if(espName.isEmpty())
        espName << QDir(folderPath).dirName() + ".esp";


    QString returnedEsp;
    int counter = 0;

    do
    {
        for(auto esp : espName)
        {
            bool texturesBsaGood = !QFile(folderPath + "/" + esp.chopped(4)
                                          + CAO_BSA_TEXTURES_SUFFIX + CAO_BSA_EXTENSION).exists()
                    && bsaType == bsaType::texturesBsa;

            bool standardBsaGood = !QFile(folderPath + "/" + esp.chopped(4)
                                          + CAO_BSA_SUFFIX + CAO_BSA_EXTENSION).exists()
                    && bsaType == bsaType::standardBsa;

            bool bothBsaGood = QFile(folderPath + "/" + esp.chopped(4) + CAO_BSA_TEXTURES_SUFFIX + CAO_BSA_EXTENSION).exists()
                    && !QFile(folderPath + "/" +esp.chopped(4) + CAO_BSA_SUFFIX + CAO_BSA_EXTENSION).exists()
                    && bsaType == bsaType::texturesAndStandardBsa;

            if(texturesBsaGood || standardBsaGood || bothBsaGood)
                returnedEsp = esp;
        }
        if(returnedEsp.isEmpty())
            espName << espName.last().chopped(4) + QString::number(counter) + ".esp";

        ++counter;
    }while(returnedEsp.isEmpty());

    return returnedEsp.remove(QRegularExpression("\\.es[plm]$"));
}

bool PluginsOperations::checkIfBsaHasPlugin(const QString& bsaPath)
{
    QString bsaName = QFileInfo(bsaPath).fileName();
    bsaName.remove(CAO_BSA_EXTENSION);
    bsaName.remove(" - Textures" + CAO_BSA_EXTENSION); // x.esp will also load x - Textures.bsa

    QString eslName = bsaName + ".esl";
    QString esmName = bsaName + ".esm";
    QString espName = bsaName + ".esp";

    bool hasEsl = QFile(eslName).exists();
    bool hasEsm = QFile(esmName).exists();
    bool hasEsp = QFile(espName).exists();

    return hasEsl || hasEsm || hasEsp;
}

QStringList PluginsOperations::listHeadparts(const QString &filepath)
{
    std::fstream file;
    file.open(filepath.toStdString(), std::ios::binary | std::ios::in);

    if(!file.is_open())
        return QStringList();

    PluginRecordHeader header;
    PluginFieldHeader pluginFieldHeader;

    QStringList headparts;

    auto readHeaders = [&]()
    {
        file.read(reinterpret_cast<char*>(&header), sizeof header);
    };

    auto readFieldPluginHeader = [&]()
    {
        file.read(reinterpret_cast<char*>(&pluginFieldHeader), sizeof pluginFieldHeader);
    };

    readHeaders();
    if(strncmp(header.plugin.type, GROUP_TES4, sizeof GROUP_TES4) != 0)
        return QStringList(); //Not a plugin file

    //Skip TES4 record
    file.seekg(header.record.dataSize, std::ios::cur);

    //Reading all groups
    do
    {
        readHeaders();

        // skip non headpart groups
        if(strncmp(header.plugin.label, GROUP_HDPT, sizeof GROUP_HDPT) != 0)
        {
            file.seekg(header.plugin.groupSize - sizeof header, std::ios::cur);
            continue;
        }

        //Reading all headpart records
        int64_t groupEndPos = header.plugin.groupSize - sizeof header + file.tellg();
        while(file.tellg() < groupEndPos)
        {
            readHeaders();

            // reading all record fields
            int64_t recEndPos =  header.record.dataSize + file.tellg() ;
            while(file.tellg() < recEndPos)
            {
                readFieldPluginHeader();
                // skip everything but MODL
                if(strncmp(pluginFieldHeader.type, GROUP_MODL, sizeof GROUP_MODL) != 0)
                {
                    file.seekg(pluginFieldHeader.dataSize, std::ios::cur);
                    continue;
                }

                char buffer[1024];
                file.read(buffer, pluginFieldHeader.dataSize);

                QString headpart = buffer;
                // make sure that nif path starts with meshes
                if(!headpart.startsWith("meshes", Qt::CaseInsensitive))
                    headpart = "meshes/" + headpart;

                //Adding headparts to the list
                headparts << QDir::cleanPath(headpart);
            }
        }
    }while (strncmp(header.plugin.type, GROUP_GRUP, sizeof GROUP_GRUP) == 0 && file.good());

    return headparts;
}
