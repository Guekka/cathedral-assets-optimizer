/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "BaseTypes.hpp"
#include "Settings.hpp"

namespace CAO {

//Note: this macro is not the same as the one standard Settings use, as types are always optional
#define REGISTER_SETTING(type, name) \
private:                             \
    type name##_ = type{};           \
                                     \
public:                              \
    const type &name() const { return name##_; }

class GameSettings final
{
public:
    static const GameSettings &get(::CAO::Games game);

private:
    GameSettings() = default;

    GameSettings(const GameSettings &) = delete;
    GameSettings(GameSettings &&)      = delete;

    GameSettings &operator=(const GameSettings &) = delete;
    GameSettings &operator=(GameSettings &&) = delete;

    REGISTER_SETTING(bsa_archive_type_t, eBSAFormat)

    REGISTER_SETTING(QString, sBSAExtension)
    REGISTER_SETTING(QString, sBSASuffix)

    REGISTER_SETTING(std::optional<bsa_archive_type_t>, eBSATexturesFormat)
    REGISTER_SETTING(std::optional<QString>, sBSATexturesSuffix)

    REGISTER_SETTING(std::optional<NiVersion>, cMeshesVersion)

    REGISTER_SETTING(std::optional<hkPackFormat>, eAnimationsFormat)
};

#undef REGISTER_SETTING

} // namespace CAO
