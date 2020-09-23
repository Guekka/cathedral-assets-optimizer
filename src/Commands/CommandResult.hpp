/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

using HRESULT = long;
#include <QString>

namespace CAO {
struct CommandResult
{
    HRESULT errorCode;
    QString errorMessage;

    bool hasError() const;
};

namespace CommandResultFactory {
CommandResult getSuccessfulResult();
CommandResult getFailedResult(HRESULT errorCode, const QString &errorMessage);
CommandResult getCannotCastFileResult();
}; // namespace CommandResultFactory

} // namespace CAO
