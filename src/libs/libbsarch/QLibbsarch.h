#pragma once

#include "libbsarch.h"
#include <string>
#include <QDir>
#include <QStringList>

#define PREPARE_PATH_LIBBSARCH(qstring) reinterpret_cast<const wchar_t *>(QDir::toNativeSeparators(qstring).utf16())

inline const std::string wcharToString(const wchar_t *text)
{
    return QString::fromWCharArray(text).toStdString();
}
