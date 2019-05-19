/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

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
