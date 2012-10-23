/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.cpp
 *  The nModules Project
 *
 *  Built-in text functions.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "IParsedText.hpp"

#define TEXTFUNCTION(x) BOOL __cdecl x(LPCWSTR name, UCHAR numArgs, LPWSTR dest, size_t cchDest)

namespace TextFunctions {
    void _Register();
    void _Unregister();

    TEXTFUNCTION(Time);
}
