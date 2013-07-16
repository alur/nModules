/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ErrorHandler.cpp
 *  The nModules Project
 *
 *  Functions for dealing with errors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "ErrorHandler.h"

// The current global error level.
static ErrorHandler::Level gErrorLevel = ErrorHandler::Level::Warning;
TCHAR gModuleName[64] = TEXT("");


/// <summary>
/// Initializes error handling.
/// </summary>
void ErrorHandler::Initialize(LPCTSTR moduleName) {
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
void ErrorHandler::Error(Level level, LPCTSTR format, ...) {
    TCHAR message[MAX_LINE_LENGTH];
    va_list argList;

    va_start(argList, format);
    StringCchVPrintf(message, MAX_LINE_LENGTH, format, argList);
    va_end(argList);

    if (gErrorLevel >= level) {
        switch (level) {
        case Level::Critical:
            MessageBox(nullptr, message, TEXT("Critical"), MB_OK | MB_ICONERROR);
            break;

        case Level::Warning:
            MessageBox(nullptr, message, TEXT("Warning"), MB_OK | MB_ICONWARNING);
            break;

        case Level::Notice:
            MessageBox(nullptr, message, TEXT("Notice"), MB_OK | MB_ICONINFORMATION);
            break;

        case Level::Debug:
            MessageBox(nullptr, message, TEXT("Debug"), MB_OK);
            break;
        }
    }
}
