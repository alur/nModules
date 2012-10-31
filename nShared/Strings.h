/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Strings.h
 *  The nModules Project
 *
 *  Utility functions for dealing with strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Strings {
    LPWSTR wcsPartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwriteW(LPWSTR dest, LPCWSTR str);
}
