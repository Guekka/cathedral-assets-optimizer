#ifndef QLIBBSARCH_H
#define QLIBBSARCH_H

#include <QStringList>
#include <QDir>
#include <string>
#include <QDebug>
#include <QMapIterator>
#include "libbsarch.h"

#include "pch.h"

const wchar_t *QStringToWchar(const QString& Qstring);
const std::string wcharToString(const wchar_t *text);


#endif // QLIBBSARCH_H


