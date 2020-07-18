/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
struct CommandResult
{
    int errorCode;
    QString errorMessage;
    bool processedFile;

    operator bool() const { return errorCode; }
};

class CommandResultFactory
{
public:
    CommandResult getSuccessfulResult() const;
    CommandResult getFailedResult(const int &errorCode, const QString &errorMessage) const;
    CommandResult getCannotCastFileResult() const;
};

} // namespace CAO
