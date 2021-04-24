/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsOptimizer.h"

void AnimationsOptimizer::convert(const QString &filePath)
{
    std::call_once(onceFlag, [this] {
        hkxcmdFound = QFile::exists("bin/hkxcmd.exe");
        if (!hkxcmdFound)
        {
            PLOG_ERROR << "HKXCMD not found. Animations won't be processed";
            return;
        }
    });

    if (!hkxcmdFound)
        return;


    QFile file(filePath);
    const QString tempHkx = "___tempAnimFile.hkx";
    QFile::remove(tempHkx);
    if (file.copy(tempHkx))
        file.remove();
    else
    {
        PLOG_ERROR << QString("Cannot copy %1 in order to convert it").arg(filePath);
        return;
    }

    QProcess hkxcmd(this);
    const QStringList args = {"convert",
                              QDir::toNativeSeparators(QFileInfo(tempHkx).absoluteFilePath()),
                              "-o",
                              QDir::toNativeSeparators(filePath),
                              "-v",
                              "AMD64"};

    hkxcmd.start("bin/hkxcmd.exe", args);
    hkxcmd.waitForFinished();

    QString tmp = hkxcmd.readAllStandardOutput() + hkxcmd.readAllStandardError();

    if (!tmp.contains("not loadable"))
        file.remove(tempHkx);
    else
    {
        PLOG_WARNING << QString("Cannot convert %1, it is probably already converted.").arg(filePath);
        QFile::rename(tempHkx, filePath);
    }
}
