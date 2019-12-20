/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "Commands/BSA/Utils/BSA.hpp"
#include "File/BSA/BSAFolder.hpp"
#include "Settings/Settings.hpp"

namespace CAO {
class BSASplit
{
public:
    static QVector<BSA> splitBSA(const QDir &dir, const Settings &settings);

protected:
    static bool isIgnoredFile(const QDir &bsaDir, const QFileInfo &fileinfo);
    static QStringList filesToNotPack;
    static void readFilesToNotPack();
    static std::once_flag onceFlag;
};
} // namespace CAO
