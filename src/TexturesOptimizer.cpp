#include "TexturesOptimizer.h"


void TexturesOptimizer::convertToBc7IfUncompressed(const QString &filePath) //Compress uncompressed textures to BC7
{
    QProcess texDiag;
    QProcess texconv;

    texDiag.start(QCoreApplication::applicationDirPath() + "/resources/texdiag.exe", QStringList {"info", filePath});
    texDiag.waitForFinished(-1);

    QString texDiagOutput = texDiag.readAllStandardOutput();

    if(texDiagOutput.contains("compressed = no"))
    {
        QString width = texDiagOutput.mid(texDiagOutput.indexOf("width = ")+8, 4);
        QString height = texDiagOutput.mid(texDiagOutput.indexOf("height = ")+9, 4);
        int textureSize = width.trimmed().toInt() * height.trimmed().toInt();

        if(textureSize > 16)
        {
            QStringList texconvArg{ "-nologo", "-y", "-m", "0", "-pow2", "-if", "FANT", "-f", "BC7_UNORM", "-bcmax", filePath};

            QLogger::QLog_Note("TexturesOptimizer", tr("Compressing uncompressed texture: ") + filePath.mid(filePath.lastIndexOf("/")+1));
            texconv.start(QCoreApplication::applicationDirPath() + "/resources/texconv.exe", texconvArg);
            texconv.waitForFinished(-1);
        }
    }
}


void TexturesOptimizer::convertTgaToDds(const QString &filePath) //Convert TGA textures to DDS
{
    QLogger::QLog_Note("TexturesOptimizer", tr("Converting TGA files...") + "\n"
    + tr("TGA file found: ") + filePath.mid(filePath.lastIndexOf("/")) + tr("Compressing..."));

    QStringList texconvArg {"-nologo", "-m", "0", "-pow2", "-if", "FANT", "-f", "R8G8B8A8_UNORM", filePath};

    QProcess texconv;
    texconv.start(QCoreApplication::applicationDirPath() + "/resources/texconv.exe", texconvArg);
    texconv.waitForFinished(-1);

    QFile tga(filePath);
    tga.remove();
}
