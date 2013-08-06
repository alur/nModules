/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Debugging.cpp
 *  The nModules Project
 *
 *  Debugging functions. Taken from the LiteStep core.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Common.h"

#ifdef _DEBUG

/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessageA(LPCSTR format, ...)
{
    ASSERT(nullptr != format);
    
    va_list args;
    char buffer[512];

    va_start(args, format);
    StringCchVPrintfExA(buffer, 512, nullptr, nullptr, STRSAFE_NULL_ON_FAILURE, format, args);
    va_end(args);
    
    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
}


/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessageW(LPCWSTR format, ...)
{
    ASSERT(nullptr != format);
    
    va_list args;
    WCHAR buffer[512];

    va_start(args, format);
    StringCchVPrintfExW(buffer, 512, nullptr, nullptr, STRSAFE_NULL_ON_FAILURE, format, args);
    va_end(args);
    
    OutputDebugStringW(buffer);
    OutputDebugStringW(L"\n");
}


/// <summary>
/// Sends a formatted message to the debug output window.
/// </summary>
void DbgTraceWindowMessage(LPCSTR prefix, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg < WM_USER)
    {
        TRACE("[%s] WM_%.4X(%.8X, %.8X)", prefix, msg, wParam, lParam);
    }
    else if (msg >= WM_USER && msg <= (WM_APP-1))
    {
        TRACE("[%s] WM_USER+%u(%.8X, %.8X)", prefix, msg - WM_USER, wParam, lParam);
    }
    else if (msg >= WM_APP && msg <= (MAXINTATOM-1))
    {
        TRACE("[%s] WM_APP+%u(%.8X, %.8X)", prefix, msg - WM_APP, wParam, lParam);
    }
    else if (msg >= MAXINTATOM)
    {
        TCHAR msgName[MAX_PATH] = { 0 };
        
        // GetClipboardFormatName retrieves the name of registered window messages too!
        if (GetClipboardFormatName(msg, msgName, _countof(msgName)) > 0)
        {
            TRACE("[%s] WM_'%s'(%.8X, %.8X)", prefix, msgName, wParam, lParam);
        }
        else
        {
            TRACE("[%s] WM_%.8X(%.8X, %.8X)", prefix, msg, wParam, lParam);
        }
    }
}

#endif /* _DEBUG */
