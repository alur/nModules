/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.cpp
 *  The nModules Project
 *
 *  Built-in text functions.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "TextFunctions.h"
#include "ParsedText.hpp"
#include <strsafe.h>
#include <ctime>


void TextFunctions::_Register() {
    RegisterDynamicTextFunction(L"Time", 0, Time, true);
    RegisterDynamicTextFunction(L"Time", 1, Time, true);
    RegisterDynamicTextFunction(L"br", 0, Br, false);
}


void TextFunctions::_Unregister() {
    UnRegisterDynamicTextFunction(L"Time", 0);
    UnRegisterDynamicTextFunction(L"Time", 1);
    UnRegisterDynamicTextFunction(L"br", 0);
}


TEXTFUNCTION(TextFunctions::Time) {
    WCHAR date[1024];
    time_t t = time(0);
    struct tm now;

    localtime_s(&now, &t);

    wcsftime(date, 1024, numArgs == 0 ? L"%H:%M" : args[0], &now);
    return SUCCEEDED(StringCchCatW(dest, cchDest, date));
}


TEXTFUNCTION(TextFunctions::Br) {
    return SUCCEEDED(StringCchCatW(dest, cchDest, L"\n"));
}
