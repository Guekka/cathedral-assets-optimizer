#include "AnimationsOptimizer.h"


void AnimationsOptimizer::optimize(const QString &filePath)
{
    QProcess havokProcess;
    QStringList havokArgs;

    havokArgs.clear();
    havokArgs << "--platformamd64" << filePath << filePath;
    havokProcess.start(QCoreApplication::applicationDirPath() + "/resources/HavokBehaviorPostProcess.exe", havokArgs);

    havokProcess.waitForFinished(-1);

    if(havokProcess.readAllStandardOutput().isEmpty())
        QLogger::QLog_Note("AnimationsOptimizer", tr("Animation successfully ported: ") + filePath);
}
