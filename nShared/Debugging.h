/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Debugging.h                                                     July, 2012
 *  The nModules Project
 *
 *  Debugging macros and functions. Taken from the LiteStep core.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <assert.h>

#define ASSERT assert

// Verify works like assert in debug, but executes the command even in release mode.
#if defined(_DEBUG)
#  define VERIFY(f)     ASSERT(f)
#  define VERIFY_HR(f)  ASSERT(SUCCEEDED(f))
#else
#  define VERIFY(f)     ((void)(f))
#  define VERIFY_HR(f)  ((void)(f))
#endif

/// <summary>
/// Sends a formatted (printf-style) message to the debug output window.
/// Automatically inserts \n at the end of the string.
/// </summary>
void DbgTraceMessage(LPCSTR pszFormat, ...);

#if defined(_DEBUG)
#  define TRACE  DbgTraceMessage
#else
#  define TRACE
#endif

#if defined(_DEBUG)
void    DbgTraceWindowMessage(LPCSTR pszPrefix, UINT uMsg, WPARAM wParam, LPARAM lParam);
#else
#define DbgTraceWindowMessage(pszPrefix, uMsg, wParam, lParam)
#endif

#endif /* DEBUGGING_H */
