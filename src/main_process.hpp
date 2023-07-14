/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <btu/modmanager/mod_folder.hpp>

[[nodiscard]] auto process_file(const btu::modmanager::ModFile &file) -> btu::modmanager::ModFile
{
    return file;
}

[[nodiscard]] auto process_mod(const btu::modmanager::ModFolder &mod) -> btu::modmanager::ModFolder
{
    return mod;
}

[[nodiscard]] auto process_mods(const std::vector<btu::modmanager::ModFolder> &mods)
    -> std::vector<btu::modmanager::ModFolder>
{
    return mods;
}
