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
}


void TextFunctions::_Unregister() {
    UnRegisterDynamicTextFunction(L"Time", 0);
}


TEXTFUNCTION(TextFunctions::Time) {
    time_t t = time(0);
    struct tm now;
    localtime_s(&now, &t);
    return SUCCEEDED(StringCchPrintfW(dest, cchDest, L"%s%02d:%02d", dest, now.tm_hour, now.tm_min));
}
