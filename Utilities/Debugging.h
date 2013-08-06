/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Debugging.h
 *  The nModules Project
 *
 *  Debugging macros and functions. Taken from the LiteStep core.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <assert.h>

#define ASSERT assert

// Verify works like assert in debug, but executes the command even in release mode.
#if defined(_DEBUG)
#   define VERIFY(f)     ASSERT(f)
#   define VERIFY_HR(f)  ASSERT(SUCCEEDED(f))
#else
#   define VERIFY(f)     ((void)(f))
#   define VERIFY_HR(f)  ((void)(f))
#endif

/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessageA(LPCSTR format, ...);
void DbgTraceMessageW(LPCWSTR format, ...);

#if defined(_DEBUG)
#   define TRACE  DbgTraceMessageA
#   define TRACEA  DbgTraceMessageA
#   define TRACEW  DbgTraceMessageW
#else
#   define TRACE
#   define TRACEA
#   define TRACEW
#endif

#if defined(_DEBUG)
void    DbgTraceWindowMessage(LPCSTR prefix, UINT msg, WPARAM wParam, LPARAM lParam);
#else
#define DbgTraceWindowMessage(prefix, msg, wParam, lParam)
#endif
