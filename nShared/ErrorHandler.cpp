/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ErrorHandler.cpp
 *  The nModules Project
 *
 *  Functions for dealing with errors.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "ErrorHandler.h"
#include "../Utilities/Error.h"

#include <strsafe.h>

// The current global error level.
static ErrorHandler::Level gErrorLevel = ErrorHandler::Level::Warning;
static wchar_t gModuleName[64] = L"";


/// <summary>
/// Initializes error handling.
/// </summary>
void ErrorHandler::Initialize(LPCWSTR moduleName) {
  StringCchCopy(gModuleName, _countof(gModuleName), moduleName);
}


/// <summary>
/// Sets the global error level.
/// </summary>
void ErrorHandler::SetLevel(Level level) {
  gErrorLevel = level;
}


/// <summary>
/// Displays a formatted error message box.
/// </summary>
/// <param name="level>The error level</param>
/// <param name="format>The message to print</param>
void ErrorHandler::Error(Level level, LPCWSTR format, ...) {
  wchar_t message[MAX_LINE_LENGTH];
  va_list argList;

  if (gErrorLevel >= level) {
    va_start(argList, format);
    StringCchVPrintf(message, MAX_LINE_LENGTH, format, argList);
    va_end(argList);

    switch (level) {
    case Level::Critical:
      MessageBox(nullptr, message, L"Critical", MB_OK | MB_ICONERROR);
      break;

    case Level::Warning:
      MessageBox(nullptr, message, L"Warning", MB_OK | MB_ICONWARNING);
      break;

    case Level::Notice:
      MessageBox(nullptr, message, L"Notice", MB_OK | MB_ICONINFORMATION);
      break;

    case Level::Debug:
      MessageBox(nullptr, message, L"Debug", MB_OK);
      break;
    }
  }
}


/// <summary>
/// Displays a formatted error message box.
/// </summary>
/// <param name="level>The error level</param>
/// <param name="format>The message to print</param>
void ErrorHandler::ErrorHR(Level level, HRESULT hr, LPCWSTR format, ...) {
  wchar_t message[MAX_LINE_LENGTH];
  va_list argList;

  if (gErrorLevel >= level) {
    *message = L'\0';

    if (format != nullptr) {
      va_start(argList, format);
      StringCchVPrintf(message, MAX_LINE_LENGTH, format, argList);
      va_end(argList);
      StringCchCat(message, _countof(message), L"\n\n");
    }

    LPTSTR end = wcschr(message, L'\0');

    DescriptionFromHR(hr, end, _countof(message) - (end - message));

    switch (level) {
    case Level::Critical:
      MessageBox(nullptr, message, L"Critical", MB_OK | MB_ICONERROR);
      break;

    case Level::Warning:
      MessageBox(nullptr, message, L"Warning", MB_OK | MB_ICONWARNING);
      break;

    case Level::Notice:
      MessageBox(nullptr, message, L"Notice", MB_OK | MB_ICONINFORMATION);
      break;

    case Level::Debug:
      MessageBox(nullptr, message, L"Debug", MB_OK);
      break;
    }
  }
}
