/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	TextFormatting.cpp												July, 2012
 *	The nModules Project
 *
 *	Formats text strings.
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nShared/Export.h"
#include "TextFormatting.h"
#include <map>
#include <ctime>

using std::map;

map<LPCSTR, FORMATTINGPROC> g_formatters;

EXPORT_CDECL(BOOL) RegisterFormatter(LPCSTR, FORMATTINGPROC) {
	return FALSE;
}

EXPORT_CDECL(BOOL) UnregisterFormatter(LPCSTR, FORMATTINGPROC) {
	return FALSE;
}

EXPORT_CDECL(BOOL) FormatText(LPCWSTR pszSource, size_t cchDest, LPWSTR pszDest) {
	time_t t = time(0);
	struct tm now;
	localtime_s(&now, &t);
	if (wcscmp(pszSource, L"time('HH:mm')") == 0) {
		return SUCCEEDED(StringCchPrintfW(pszDest, cchDest, L"%02d:%02d", now.tm_hour, now.tm_min));
	}
	else {
		return SUCCEEDED(StringCchCopyW(pszDest, cchDest, pszSource));
	}
}
