/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsOptimizer.h"

void AnimationsOptimizer::convert(const QString &filePath, const hkPackFormat &pkFormat)
{
    if (pkFormat != HKPF_AMD64)
        return;

    QFile file(filePath);
    const QString tempHkx = "___tempAnimFile.hkx";
    file.copy(tempHkx);
    file.remove();

    QProcess hkxcmd(this);
    hkxcmd.setProgram("bin/hkxcmd.exe");
    const QStringList args = {"convert", "___tempAnimFile.hkx", "-o", filePath, "-v", "AMD64"};
    hkxcmd.setArguments(args);

    hkxcmd.start();
    hkxcmd.waitForFinished();

    file.remove(tempHkx);
}
