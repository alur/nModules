/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StringUtils.h
 *  The nModules Project
 *
 *  Utilities for dealing with strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <vector>

namespace StringUtils
{
    LPSTR PartialDup(LPCSTR str, size_t cch);
    LPWSTR PartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);
}
