/*!
  * Stripped down version of this file https://github.com/aerisarn/ck-cmd/blob/master/src/commands/hkx/Convert.cpp
  */

#include "AnimationsConvert.hpp"
#include "File/Animations/AnimationFile.hpp"
#include "Settings/Games.hpp"

#include <QProcess>

namespace CAO {

AnimationsConvert::AnimationsConvert()
{
    hkxcmdFound = QFile::exists(hkxcmdPath);
    if (!hkxcmdFound)
    {
        PLOG_ERROR << "HKXCMD not found. Animations won't be processed";
        return;
    }
}

CommandResult AnimationsConvert::process(File &file) const
{
    auto *havok = file.getFile<Resources::Animation>(true);
    if (!havok)
        return CommandResultFactory::getCannotCastFileResult();

    if (!hkxcmdFound)
        return CommandResultFactory::getFailedResult(1, "HKXCMD not found");

    const QString tempHkx = "___tempAnimFile.hkx";
    const QString outHkx  = "___tempAnimFile-out.hkx";

    QFile::remove(tempHkx);
    QFile::remove(outHkx);

    const auto filepath = file.getInputFilePath();
    QFile qfile(filepath);
    if (!qfile.copy(tempHkx))
    {
        return CommandResultFactory::getFailedResult(2,
                                                     QString("Cannot copy %1 in order to convert it")
                                                         .arg(filepath));
    }

    QProcess hkxcmd;
    const QString tempHkxFull = QDir::toNativeSeparators(QFileInfo(tempHkx).absoluteFilePath());
    const QStringList args    = {"convert", tempHkxFull, "-v", "AMD64"};

    hkxcmd.start(hkxcmdPath, args);
    hkxcmd.waitForFinished();

    const QString output = hkxcmd.readAllStandardOutput() + hkxcmd.readAllStandardError();
    const bool success   = !output.contains("not loadable");

    if (!success)
    {
        return CommandResultFactory::getFailedResult(
            3, QString("Cannot convert %1, it is probably already converted.").arg(filepath));
    }

    QFile::remove(filepath);
    if (!QFile::rename(outHkx, file.getOutputFilePath()))
    {
        return CommandResultFactory::getFailedResult(
            4, QString("Failed to convert %1: Cannot copy it back to its path").arg(filepath));
    }

    return CommandResultFactory::getSuccessfulResult();
}

CommandState AnimationsConvert::isApplicable(File &file) const
{
    if (!file.patternSettings().bAnimationsOptimization())
        return CommandState::NotRequired;

    const bool convert = GameSettings::get(currentProfile().getGeneralSettings().eGame()).bAnimationsConvert();
    if (!convert)
        return CommandState::NotRequired;

    const auto *havok = file.getFile<Resources::Animation>();
    if (!havok)
        return CommandState::NotRequired;

    return CommandState::Ready;
}

} // namespace CAO
