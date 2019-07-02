#pragma once

#include <QStringList>
#include <QDir>
#include <string>
#include "libbsarch.h"

const wchar_t *QStringToWchar(const QString& Qstring);
const std::string wcharToString(const wchar_t *text);
