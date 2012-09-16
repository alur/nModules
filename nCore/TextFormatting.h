/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TextFormatting.h                                              August, 2012
 *  The nModules Project
 *
 *  Formats text strings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TEXTFORMATTING_H
#define TEXTFORMATTING_H

typedef VOID (__cdecl * FORMATTINGPROC)(HWND hwndOwner, LPCSTR pszArgs);
EXPORT_CDECL(BOOL) RegisterTextFormatter(LPCSTR, FORMATTINGPROC);
EXPORT_CDECL(BOOL) UnregisterTextFormatter(LPCSTR, FORMATTINGPROC);

FORMATTINGPROC FindFormatter(LPCSTR pszFunc);

// Valid formats:
// [Function]
// [Function(stuff)]
// [Function('some stuff')]
// [if(function)] stuff [else] other stuff [endif]

#endif /* TEXTFORMATTING_H */