#pragma once
#ifndef _WINDOWS_
#  include <windows.h>
#endif
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <map>

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <malloc.h>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

typedef struct EnumLookupType {
    int value;
    const char *name;
} EnumLookupType;
