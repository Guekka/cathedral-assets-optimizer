/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BSA/BSAFolder.hpp"
#include "BSA/Utils/BSA.hpp"
#include "OptionsCAO.hpp"

namespace CAO {
class BSASplit
{
public:
    static QVector<BSA> splitBSA(const QDir &dir);

protected:
    static bool isIgnoredFile(const QDir &bsaDir, const QFileInfo &fileinfo);
    static QStringList filesToNotPack;
    static void readFilesToNotPack();
};
} // namespace CAO
