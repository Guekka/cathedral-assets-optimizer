/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "Commands/CommandResult.hpp"

namespace CAO {
CommandResult CommandResultFactory::getSuccessfulResult()
{
    CommandResult result;
    result.errorCode = 0;
    result.errorMessage.clear();
    result.processedFile = true;
    return result;
}

CommandResult CommandResultFactory::getFailedResult(const int &errorCode, const QString &errorMessage)
{
    return CommandResult{errorCode, errorMessage, false};
}

CommandResult CommandResultFactory::getCannotCastFileResult()
{
    CommandResult result;
    result.errorCode = 3;
    result.errorMessage = "Failed to cast File to its derived type";
    result.processedFile = false;
    return result;
}
} // namespace CAO
