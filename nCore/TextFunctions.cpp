/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.cpp
 *  The nModules Project
 *
 *  Built-in text functions.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "TextFunctions.h"
#include "ParsedText.hpp"
#include <strsafe.h>
#include <ctime>


static size_t __cdecl Time(LPCWSTR /* name */, UCHAR numArgs, LPWSTR *args, LPWSTR dest, size_t cchDest) {
  std::time_t t = std::time(nullptr);
  std::tm now;
  localtime_s(&now, &t);
  return std::wcsftime(dest, cchDest, numArgs == 0 ? L"%H:%M" : args[0], &now);
}


void TextFunctions::_Register() {
  RegisterDynamicTextFunction(L"Time", 0, Time, true);
  RegisterDynamicTextFunction(L"Time", 1, Time, true);

  RegisterDynamicTextFunction(L"WindowTitle", 1, [] (LPCWSTR /* name */, UCHAR /* numArgs */, LPWSTR* args, LPWSTR dest, size_t cchDest) -> size_t {
    HWND window = FindWindowW(args[0], nullptr);
    if (window) {
      return GetWindowTextW(window, dest, (int)cchDest);
    }
    return 0;
  }, true);
}


void TextFunctions::_Unregister() {
  UnRegisterDynamicTextFunction(L"Time", 0);
  UnRegisterDynamicTextFunction(L"Time", 1);
  UnRegisterDynamicTextFunction(L"WindowTitle", 1);
}
