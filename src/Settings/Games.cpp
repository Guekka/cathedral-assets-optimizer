/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Games.hpp"

#include "Utils/CallOnce.hpp"

namespace CAO {
namespace Dummy {
static constexpr uint8_t Skyrim[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x2B, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                     0x9A, 0x99, 0xD9, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                     0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t SkyrimSE[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x02,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x2C, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                       0x9A, 0x99, 0xD9, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                       0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t Oblivion[] = {0x54, 0x45, 0x53, 0x34, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x45,
                                       0x44, 0x52, 0x0C, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x08, 0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x08, 0x00,
                                       0x44, 0x45, 0x46, 0x41, 0x55, 0x4C, 0x54, 0x00};

static constexpr uint8_t FNV[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x0F, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                  0x1F, 0x85, 0xAB, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                  0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t Fallout4[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x02,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x83, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                       0x33, 0x33, 0x73, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                       0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};
} // namespace Dummy

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
                sets.sDummyPlugin_       = std::nullopt;
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.bAnimationsConvert_ = false;
            });

            return sets;
        }
        case Games::Oblivion:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_    = baTES4;
                sets.sBSAExtension_ = ".bsa";
                sets.sBSASuffix_    = ".bsa";
                sets.sDummyPlugin_->assign(std::begin(Dummy::Oblivion), std::end(Dummy::Oblivion));
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.bAnimationsConvert_ = false;
            });

            return sets;
        }
        case Games::SkyrimLE:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_    = baFO3;
                sets.sBSAExtension_ = ".bsa";
                sets.sBSASuffix_    = ".bsa";
                sets.sDummyPlugin_->assign(std::begin(Dummy::Skyrim), std::end(Dummy::Skyrim));
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = nifly::NiVersion::getSK();

                //In theory, the following should be used
                //sets.eAnimationsFormat_  = hkPackFormat::HKPF_WIN32;
                //But the used Havok SDK does not support it

                sets.bAnimationsConvert_ = false;
            });

            return sets;
        }
        case Games::SkyrimSE:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_    = baSSE;
                sets.sBSAExtension_ = ".bsa";
                sets.sBSASuffix_    = ".bsa";
                sets.sDummyPlugin_->assign(std::begin(Dummy::SkyrimSE), std::end(Dummy::SkyrimSE));
                sets.eBSATexturesFormat_ = baSSE;
                sets.sBSATexturesSuffix_ = " - Textures.bsa";
                sets.cMeshesVersion_     = nifly::NiVersion::getSSE();
                sets.bAnimationsConvert_ = true;
            });

            return sets;
        }
        case Games::Fallout3:
        case Games::FalloutNewVegas:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_    = baFO3;
                sets.sBSAExtension_ = ".bsa";
                sets.sBSASuffix_    = ".bsa";
                sets.sDummyPlugin_->assign(std::begin(Dummy::FNV), std::end(Dummy::FNV));
                sets.eBSATexturesFormat_ = std::nullopt;
                sets.sBSATexturesSuffix_ = std::nullopt;
                sets.cMeshesVersion_     = std::nullopt;
                sets.bAnimationsConvert_ = false;
            });

            return sets;
        }
        case Games::Fallout4:
        {
            static GameSettings sets;
            static CallOnce callOnce;
            callOnce.call([] {
                sets.eBSAFormat_    = baFO4;
                sets.sBSAExtension_ = ".bsa";
                sets.sBSASuffix_    = " - Main.bsa";
                sets.sDummyPlugin_->assign(std::begin(Dummy::Fallout4), std::end(Dummy::Fallout4));
                sets.eBSATexturesFormat_ = baFO4dds;
                sets.sBSATexturesSuffix_ = " - Textures.bsa";
                sets.cMeshesVersion_     = std::nullopt;
                sets.bAnimationsConvert_ = false;
            });

            return sets;
        }
    }
    //Never reached
    return get(Games::SkyrimSE);
}
} // namespace CAO
