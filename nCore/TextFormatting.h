/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.h
 *  The nModules Project
 *
 *  Formats text strings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../nShared/Macros.h"

typedef BOOL (__cdecl * FORMATTINGPROC)(LPCWSTR name, UCHAR numArgs, LPCWSTR arguments[], bool* isDynamic);

EXPORT_CDECL(BOOL) RegisterTextFormatter(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter);
EXPORT_CDECL(BOOL) UnregisterTextFormatter(LPCWSTR name, UCHAR numArgs);


// Valid formats:
// [Function]
// [Function(stuff)]
// [Function('some stuff')]
// [if(function)] stuff [else] other stuff [endif]


// Basic text formatting idea:
// 1. Split string into things that look like functions
// 2. Go through each string
//    -> if it is not a function, append to result
//    -> if it is a function, try to find a handler for it, if no handler is found, append to result.
// 3. Call the handler. 
