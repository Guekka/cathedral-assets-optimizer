/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Games.hpp"
#include "Utils/CallOnce.hpp"

namespace CAO {
const GameSettings &GameSettings::get(Games game)
{
    switch (game)
    {
        case Games::Morrowind:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baTES3;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = ".bsa";
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.eAnimationsFormat_  = std::nullopt;
            });

            return sets;
        }
        case Games::Oblivion:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baTES4;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = ".bsa";
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.eAnimationsFormat_  = std::nullopt;
            });

            return sets;
        }
        case Games::SkyrimLE:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baFO3;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = ".bsa";
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = NiVersion::getSK();

                //In theory, the following should be used
                //sets.eAnimationsFormat_  = hkPackFormat::HKPF_WIN32;
                //But the used Havok SDK does not support it

                sets.eAnimationsFormat_ = std::nullopt;
            });

            return sets;
        }
        case Games::SkyrimSE:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baSSE;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = ".bsa";
                sets.eBSATexturesFormat_ = baSSE;
                sets.sBSATexturesSuffix_ = " - Textures.bsa";
                sets.cMeshesVersion_     = NiVersion::getSSE();
                sets.eAnimationsFormat_  = hkPackFormat::HKPF_AMD64;
            });

            return sets;
        }
        case Games::Fallout3:
        case Games::FalloutNewVegas:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baFO3;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = ".bsa";
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.eAnimationsFormat_  = std::nullopt;
            });

            return sets;
        }
        case Games::Fallout4:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_         = baFO4;
                sets.sBSAExtension_      = ".bsa";
                sets.sBSASuffix_         = " - Main.bsa";
                sets.eBSATexturesFormat_ = baFO4dds;
                sets.sBSATexturesSuffix_ = " - Textures.bsa";
                sets.cMeshesVersion_     = std::nullopt;
                sets.eAnimationsFormat_  = std::nullopt;
            });

            return sets;
        }
    }
    //Never reached
    return get(Games::SkyrimSE);
}
} // namespace CAO
