/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	TextFormatting.h												July, 2012
 *	The nModules Project
 *
 *	Formats text strings.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef VOID (__cdecl * FORMATTINGPROC)(HWND hwndOwner, LPCSTR pszArgs);
EXPORT_CDECL(BOOL) RegisterTextFormatter(LPCSTR, FORMATTINGPROC);
EXPORT_CDECL(BOOL) UnregisterTextFormatter(LPCSTR, FORMATTINGPROC);

// Valid formats:
// [Function]
// [Function(stuff)]
// [Function('some stuff')]
// [if(function)] stuff [else] other stuff [endif]
