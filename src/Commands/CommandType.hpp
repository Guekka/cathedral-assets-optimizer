/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "pch.hpp"

namespace CAO {
Q_NAMESPACE
enum class CommandType
{
    Invalid = -1,
    Texture,
    Mesh,
    BSAFile,
    BSAFolder,
    Animation,
    Plugin
};
Q_ENUM_NS(CommandType)
} // namespace CAO
Q_DECLARE_METATYPE(CAO::CommandType)
