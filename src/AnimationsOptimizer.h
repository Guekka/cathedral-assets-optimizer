/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.h"
/*!
 * \brief The AnimationsOptimizer class will handle all operations related to animations (hkx files)
 */

class AnimationsOptimizer : public QObject
{
    Q_DECLARE_TR_FUNCTIONS(AnimationsOptimizer)

public:
    AnimationsOptimizer();
    ~AnimationsOptimizer();
    /*!
     * \brief Port an Oldrim animation to Skyrim Special Edition using Bethesda Havok Post Processing Tool
     * \param filePath The path of the file to optimize
     */
    void convert(const QString &filePath, const hkPackFormat &pkFormat);

private:
    hkMemoryRouter *memoryRouter;
};
