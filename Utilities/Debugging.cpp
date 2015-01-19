//-------------------------------------------------------------------------------------------------
// /Utilities/Debugging.cpp
// The nModules Project
//
// Debugging functions. Taken from the LiteStep core.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "Error.h"

#include <strsafe.h>

#ifdef _DEBUG


/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessageA(LPCSTR format, ...) {
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
void DbgTraceMessageW(LPCWSTR format, ...) {
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
void DbgTraceWindowMessage(LPCSTR prefix, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg < WM_USER) {
    TRACE("[%s] WM_%.4X(%p, %p)", prefix, msg, wParam, lParam);
  } else if (msg >= WM_USER && msg <= (WM_APP-1)) {
    TRACE("[%s] WM_USER+%u(%p, %p)", prefix, msg - WM_USER, wParam, lParam);
  } else if (msg >= WM_APP && msg <= (MAXINTATOM-1)) {
    TRACE("[%s] WM_APP+%u(%p, %p)", prefix, msg - WM_APP, wParam, lParam);
  } else if (msg >= MAXINTATOM) {
    TCHAR msgName[MAX_PATH] = { 0 };

    // GetClipboardFormatName retrieves the name of registered window messages too!
    if (GetClipboardFormatName(msg, msgName, MAX_PATH) > 0) {
      TRACE("[%s] WM_'%s'(%p, %p)", prefix, msgName, wParam, lParam);
    } else {
      TRACE("[%s] WM_%.8X(%p, %p)", prefix, msg, wParam, lParam);
    }
  }
}


/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceHr(LPCWSTR prefixFormat, HRESULT hr, ...) {
  ASSERT(nullptr != prefixFormat);

  va_list args;
  WCHAR buffer[512];

  va_start(args, prefixFormat);
  StringCchVPrintfExW(buffer, 512, nullptr, nullptr, STRSAFE_NULL_ON_FAILURE, prefixFormat, args);
  va_end(args);
  OutputDebugStringW(buffer);

  DescriptionFromHR(hr, buffer, 512);
  OutputDebugStringW(buffer);

  OutputDebugStringW(L"\n");
}

#endif /* _DEBUG */
