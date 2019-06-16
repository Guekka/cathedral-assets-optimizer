/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "TexturesOptimizer.h"


void TexturesOptimizer::compress(const QString &filePath) //Compress uncompressed textures to BC7
{
    QProcess texDiag;
    texDiag.start("resources/texdiag.exe", QStringList {"info", filePath});
    texDiag.waitForFinished(-1);

    QString texDiagOutput = texDiag.readAllStandardOutput();

    if(texDiagOutput.contains("compressed = no"))
    {
        QString width = texDiagOutput.mid(texDiagOutput.indexOf("width = ")+8, 4);
        QString height = texDiagOutput.mid(texDiagOutput.indexOf("height = ")+9, 4);
        int textureSize = width.trimmed().toInt() * height.trimmed().toInt();

        if(textureSize > 16)
        {
            PLOG_INFO << tr("Compressing uncompressed texture: ") + filePath;
            convertTexture(filePath, CAO_TEXTURES_FORMAT);
        }
    }
}

bool TexturesOptimizer::convertIncompatibleTextures(const QString &filePath) //Convert TGA textures to DDS
{
    if(isIncompatible(filePath))
    {
        PLOG_INFO << tr("Incompatible texture found: ")
                           + filePath.mid(filePath.lastIndexOf("/")) + "\n" + tr("Compressing...");

        convertTexture(filePath, CAO_TEXTURES_FORMAT);

        return true;
    }
    else
        return false;
}


bool TexturesOptimizer::isCompressed(const QString &filePath)
{
    QProcess texDiag;
    texDiag.start("resources/texdiag.exe", QStringList {"info", filePath});
    texDiag.waitForFinished(-1);

    return texDiag.readAllStandardOutput().contains("compressed = no");
}

QString TexturesOptimizer::getFormat(const QString &filePath)
{
    QProcess texDiag;
    texDiag.start("resources/texdiag.exe", QStringList {"info", filePath});
    texDiag.waitForFinished(-1);

    QString output = texDiag.readAllStandardOutput();

    QString format = output.section('\r', 10, 10).remove("format = ").trimmed();
    qDebug() << "format:" << format;
    return format;
}

void TexturesOptimizer::convertTexture(const QString &filePath, const QString &format)
{
    QStringList texconvArg {"-nologo", "-m", "0", "-pow2", "-if", "FANT", "-f", format, "-bcmax", "-y", filePath};

    QProcess texconv;
    texconv.start("resources/texconv.exe", texconvArg);
    texconv.waitForFinished(-1);
}

bool TexturesOptimizer::isIncompatible(const QString &filePath)
{
    bool isIncompatible = false;
    //Checking incompatibility with extension
    for(const auto& extension : CAO_TEXTURES_INCOMPATIBLE_EXTENSIONS)
        if(filePath.endsWith(extension, Qt::CaseInsensitive))
        {
            isIncompatible = true;
            break;
        }

    //Checking incompatibility with file format
    QString fileFormat = getFormat(filePath);
    for(const auto& format : CAO_TEXTURES_INCOMPATIBLE_FORMATS)
        if(fileFormat.compare(format) == 0)
        {
            isIncompatible = true;
            break;
        }

    return isIncompatible;
}
