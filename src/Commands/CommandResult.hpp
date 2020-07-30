/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "pch.hpp"

namespace CAO {
struct CommandResult
{
    HRESULT errorCode;
    QString errorMessage;
    bool processedFile;

    bool hasError() const { return FAILED(errorCode); }
};

class CommandResultFactory
{
public:
    CommandResult getSuccessfulResult() const;
    CommandResult getFailedResult(HRESULT errorCode, const QString &errorMessage) const;
    CommandResult getCannotCastFileResult() const;
};

} // namespace CAO
