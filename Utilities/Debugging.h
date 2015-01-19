//-------------------------------------------------------------------------------------------------
// /Utilities/Debugging.h
// The nModules Project
//
// Debugging functions. Taken from the LiteStep core.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../nShared/BuildOptions.h"

extern "C" _CRTIMP void __cdecl _wassert(
  _In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);

#if defined(BUILDOPTIONS_ASSERTS) || defined(_DEBUG)
#define ASSERT(_Expression) (void)((!!(_Expression)) || \
      (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0))
#else
#define ASSERT
#endif

// Verify works like assert in debug, but executes the command even in release mode.
#if defined(BUILDOPTIONS_ASSERTS) || defined(_DEBUG)
#define VERIFY(f)     ASSERT(f)
#define VERIFY_HR(f)  ASSERT(SUCCEEDED(f))
#else
#define VERIFY(f)     ((void)(f))
#define VERIFY_HR(f)  ((void)(f))
#endif

/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessageA(LPCSTR format, ...);
void DbgTraceMessageW(LPCWSTR format, ...);

#if defined(_DEBUG)
#define TRACE  DbgTraceMessageA
#define TRACEA  DbgTraceMessageA
#define TRACEW  DbgTraceMessageW
#else
#define TRACE
#define TRACEA
#define TRACEW
#endif

#if defined(_DEBUG)
void DbgTraceHr(LPCWSTR prefixFormat, HRESULT hr, ...);
#else
#define DbgTraceHr
#endif

#if defined(_DEBUG)
void    DbgTraceWindowMessage(LPCSTR prefix, UINT msg, WPARAM wParam, LPARAM lParam);
#else
#define DbgTraceWindowMessage(prefix, msg, wParam, lParam)
#endif
