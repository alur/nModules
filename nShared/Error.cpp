/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Error.cpp
 *  The nModules Project
 *
 *  Functions for dealing with errors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "Error.h"

// The current global error level.
ERRORLVL errorLevel = E_LVL_WARNING;


/// <summary>
/// Sets the global error level.
/// </summary>
void SetErrorLevel(ERRORLVL nLevel) {
    errorLevel = nLevel;
}


/// <summary>
/// Displays a formatted error message box.
/// </summary>
/// <param name="nLevel>The error level</param>
void ErrorMessage(ERRORLVL nLevel, LPCTSTR pszFormat, ...) {
    TCHAR szError[MAX_LINE_LENGTH];
    va_list argList;

    va_start(argList, pszFormat);
    StringCchVPrintf(szError, MAX_LINE_LENGTH, pszFormat, argList);
    va_end(argList);

    if (errorLevel >= nLevel) {
        switch (nLevel) {
        case E_LVL_ERROR:
            MessageBox(NULL, szError, TEXT("Error"), MB_OK | MB_ICONERROR);
            break;
        case E_LVL_WARNING:
            MessageBox(NULL, szError, TEXT("Warning"), MB_OK | MB_ICONWARNING);
            break;
        case E_LVL_NOTICE:
            MessageBox(NULL, szError, TEXT("Notice"), MB_OK | MB_ICONINFORMATION);
            break;
        case E_LVL_DEBUG:
            MessageBox(NULL, szError, TEXT("Debug"), MB_OK);
            break;
        }
    }
}
