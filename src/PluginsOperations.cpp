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

        if(!checkIfBsaHasPlugin(it.filePath()) && it.fileName().endsWith(".bsa", Qt::CaseInsensitive))
        {
            if(it.fileName().contains("- Textures", Qt::CaseInsensitive))
            {
                espName = it.fileName().remove(" - Textures.bsa", Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created textures bsa plugin:" + espName);
            }

            else
            {
                espName = it.fileName().remove(".bsa", Qt::CaseInsensitive) + ".esp";
                QLogger::QLog_Trace("PluginsOperations", "Created standard bsa plugin:" + espName);
            }
            QFile::copy(QCoreApplication::applicationDirPath() + "/resources/BlankSSEPlugin.esp", folderPath + "/" + espName);
        }
    }
    QLogger::QLog_Trace("PluginsOperations", "Exiting makeDummyPlugins function");
}


QString PluginsOperations::findPlugin(const QString& folderPath, bsaType bsaType)
{
    QDirIterator it(folderPath);
    QStringList espName;
    QString bsaName;

    while (it.hasNext())
    {
        it.next();

        if(it.fileName().contains(QRegularExpression("\\.es[plm]$")))
            espName << it.fileName();


        if(it.fileName().endsWith(".bsa", Qt::CaseInsensitive) && !it.fileName().endsWith(" - Textures.bsa", Qt::CaseInsensitive))
            bsaName = it.fileName().chopped(4) + ".esp";
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
            bool texturesBsaGood = !QFile(folderPath + "/" + esp.chopped(4) + " - Textures.bsa").exists() && bsaType == bsaType::texturesBsa;
            bool standardBsaGood = !QFile(folderPath + "/" +esp.chopped(4) + ".bsa").exists() && bsaType == bsaType::standardBsa;
            bool bothBsaGood = QFile(folderPath + "/" + esp.chopped(4) + " - Textures.bsa").exists()
                    && !QFile(folderPath + "/" +esp.chopped(4) + ".bsa").exists()
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
    bsaName.remove(".bsa");
    bsaName.remove(" - Textures.bsa"); // x.esp will also load x - Textures.bsa

    QString eslName = bsaName + ".esl";
    QString esmName = bsaName + ".esm";
    QString espName = bsaName + ".esp";

    bool hasEsl = QFile(eslName).exists();
    bool hasEsm = QFile(esmName).exists();
    bool hasEsp = QFile(espName).exists();

    return hasEsl || hasEsm || hasEsp;
}
