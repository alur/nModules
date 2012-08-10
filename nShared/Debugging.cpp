/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Debugging.cpp                                                   July, 2012
 *  The nModules Project
 *
 *  Debugging functions. Taken from the LiteStep core.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include <Windows.h>
#include "Debugging.h"

// Count array elements
#define COUNTOF(array) (sizeof(array)/sizeof(array[0]))

#ifdef _DEBUG


/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessage(LPCSTR pszFormat, ...) {
    ASSERT(NULL != pszFormat);
    
    va_list args;
    char szBuffer[512];

    va_start(args, pszFormat);
    StringCchVPrintfExA(szBuffer, 512, NULL, NULL, STRSAFE_NULL_ON_FAILURE, pszFormat, args);
    va_end(args);
    
    OutputDebugStringA(szBuffer);
    OutputDebugStringA("\n");
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// DbgTraceWindowMessage
//
void DbgTraceWindowMessage(LPCSTR pszPrefix, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg < WM_USER) {
        TRACE("[%s] WM_%.4X(%.8X, %.8X)", pszPrefix, uMsg, wParam, lParam);
    }
    else if (uMsg >= WM_USER && uMsg <= (WM_APP-1)) {
        TRACE("[%s] WM_USER+%u(%.8X, %.8X)", pszPrefix, uMsg-WM_USER, wParam, lParam);
    }
    else if (uMsg >= WM_APP && uMsg <= (MAXINTATOM-1)) {
        TRACE("[%s] WM_APP+%u(%.8X, %.8X)", pszPrefix, uMsg-WM_APP, wParam, lParam);
    }
    else if (uMsg >= MAXINTATOM) {
        TCHAR szMsgName[MAX_PATH] = { 0 };
        
        // GetClipboardFormatName retrieves the name of registered window messages too!
        if (GetClipboardFormatName(uMsg, szMsgName, COUNTOF(szMsgName)) > 0) {
            TRACE("[%s] WM_'%s'(%.8X, %.8X)", pszPrefix, szMsgName, wParam, lParam);
        }
        else {
            TRACE("[%s] WM_%.8X(%.8X, %.8X)", pszPrefix, uMsg, wParam, lParam);
        }
    }
}

#endif /* _DEBUG */
