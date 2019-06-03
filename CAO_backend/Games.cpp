/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Games.h"

void Games::setGame(const GameMode &newGame)
{
    game = newGame;
}

bsa_archive_type_e Games::GetBsaFormat()
{
    switch (game)
    {
    case Oldrim: return  bsa_archive_type_e::baFO3;
    case SSE: return  bsa_archive_type_e::baSSE;
    }

    return bsa_archive_type_e();
}

double Games::getBsaMaxSize()
{
    switch (game)
    {
    case Oldrim: return 2107483647.0;
    case SSE: return 2107483647.0;
    }

    return double();
}

double Games::getBsaTexturesMaxSize()
{
    switch (game)
    {
    case Oldrim: return 2547483647.0;
    case SSE: return 2547483647.0;
    }

    return double();
}

QString Games::getBsaExtension()
{
    //if(game == Fallout4)
    return ".bsa";
}

uint Games::getMeshesUser()
{
    switch (game)
    {
    case Oldrim: return 12;
    case SSE: return 12;
    }

    return int();
}

uint Games::getMeshesStream()
{
    switch (game)
    {
    case Oldrim: return 83;
    case SSE: return 100;
    }

    return int();
}

NiFileVersion Games::getMeshesFileVersion()
{
    switch (game)
    {
    case Oldrim: return  V20_2_0_7;
    case SSE: return  V20_2_0_7;
    }

    return NiFileVersion();
}

QString Games::getTexturesFormat()
{
    switch (game)
    {
    case Oldrim: return "BC3_UNORM";
    case SSE: return "BC7_UNORM";
    }

    return QString();
}

hkPackFormat Games::getAnimationsFormat()
{
    switch (game)
    {
    case Oldrim: return HKPF_WIN32;
    case SSE: return HKPF_AMD64;
    }

    return hkPackFormat();
}



