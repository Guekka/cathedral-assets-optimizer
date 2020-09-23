/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/CommandResult.hpp"

namespace CAO {
bool CommandResult::hasError() const
{
    return errorCode < 0;
}
} // namespace CAO

namespace CAO::CommandResultFactory {
CommandResult getSuccessfulResult()
{
    CommandResult result;
    result.errorCode = 0;
    result.errorMessage.clear();
    return result;
}

CommandResult getFailedResult(HRESULT errorCode, const QString &errorMessage)
{
    //Error code are always negative
    if (errorCode > 0)
        errorCode = -errorCode;

    return CommandResult{errorCode, errorMessage};
}

CommandResult getCannotCastFileResult()
{
    CommandResult result;
    result.errorCode     = 3;
    result.errorMessage  = "Failed to cast File to its derived type";
    return result;
}
} // namespace CAO::CommandResultFactory
