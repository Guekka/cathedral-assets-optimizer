/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsOptimizer.h"

void AnimationsOptimizer::convert(const QString &filePath)
{
    std::call_once(onceFlag, [this] {
        hkxcmdFound = QFile::exists(hkxcmdPath);
        if (!hkxcmdFound)
        {
            PLOG_ERROR << "HKXCMD not found. Animations won't be processed";
            return;
        }
    });

    if (!hkxcmdFound)
        return;

    const QString tempHkx = "___tempAnimFile.hkx";
    const QString outHkx = "___tempAnimFile-out.hkx";

    QFile::remove(tempHkx);
    QFile::remove(outHkx);

    QFile file(filePath);
    if (!file.copy(tempHkx)) {
        PLOG_ERROR << QString("Cannot copy %1 in order to convert it").arg(filePath);
        return;
    }

    QProcess hkxcmd(this);
    const QString tempHkxFull = QDir::toNativeSeparators(QFileInfo(tempHkx).absoluteFilePath());
    const QStringList args = {"convert", tempHkxFull, "-v", "AMD64"};

    hkxcmd.start(hkxcmdPath, args);
    hkxcmd.waitForFinished();

    const QString output = hkxcmd.readAllStandardOutput() + hkxcmd.readAllStandardError();
    const bool success = !output.contains("not loadable");

    if (!success) {
        PLOG_WARNING << QString("Cannot convert %1, it is probably already converted.").arg(filePath);
        return;
    }

    QFile::remove(filePath);
    if (!QFile::rename(outHkx, filePath)) {
        PLOG_ERROR << QString("Failed to convert %1: Cannot copy it back to its path").arg(filePath);
        return;
    }

    PLOG_INFO << QString("Successfully converted %1").arg(filePath);
}
