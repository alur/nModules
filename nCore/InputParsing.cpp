/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	InputParsing.cpp												July, 2012
 *	The nModules Project
 *
 *	Utility functions for parsing input
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nShared/Export.h"
#include "KnownColors.h"

using Gdiplus::ARGB;

/// <summary>
///	Parses a user inputed coordinate.
/// </summary>
/// <param name="pszCoordinate">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid coordinate.</param>
/// <param name="canBeRelative">Ttrue if the coordinate can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the coordinate can be negative.</param>
/// <returns>True if szCoordinate is a valid coordinate.</return>
EXPORT_CDECL(bool) ParseCoordinateEx(LPCSTR pszCoordinate, int * target, bool /* canBeRelative */ = true, bool canBeNegative = true) {
	char * endPtr;
	int i = strtol(pszCoordinate, &endPtr, 0);

	if (*pszCoordinate == '\0' || *endPtr != '\0' || (!canBeNegative && i < 0)) {
		return false;
	}

	*target = i;

	return true;
}

/// <summary>
///	Parses a user inputed length.
/// </summary>
/// <param name="pszLength">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid length.</param>
/// <param name="canBeRelative">Ttrue if the length can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the length can be negative.</param>
/// <returns>True if szLength is a valid length.</return>
EXPORT_CDECL(bool) ParseLength(LPCSTR pszLength, int * target, bool /* canBeRelative */ = true, bool canBeNegative = false) {
	char * endPtr;
	int i = strtol(pszLength, &endPtr, 0);

	if (*pszLength == '\0' || *endPtr != '\0' || (!canBeNegative && i < 0)) {
		return false;
	}

	*target = i;

	return true;
}

/// <summary>
/// Reads a prefixed integer value from an RC file
/// </summary>
EXPORT_CDECL(int) GetPrefixedRCInt(LPCSTR szPrefix, LPCSTR szOption, int nDefault) {
	char szOptionName[MAX_LINE_LENGTH];
	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	return GetRCInt(szOptionName, nDefault);
}

/// <summary>
/// Parses a monitor definition
/// </summary>
EXPORT_CDECL(bool) ParseMonitor(LPCSTR pszMonitor, UINT * target) {
	if (_stricmp("primary", pszMonitor) == 0) *target = 0;
	else if (_stricmp("secondary", pszMonitor) == 0) *target = 1;
	else if (_stricmp("tertiary", pszMonitor) == 0) *target = 2;
	else if (_stricmp("quaternary", pszMonitor) == 0) *target = 3;
	else if (_stricmp("quinary", pszMonitor) == 0) *target = 4;
	else if (_stricmp("senary", pszMonitor) == 0) *target = 5;
	else if (_stricmp("septenary", pszMonitor) == 0) *target = 6;
	else if (_stricmp("octonary", pszMonitor) == 0) *target = 7;
	else if (_stricmp("nonary", pszMonitor) == 0) *target = 8;
	else if (_stricmp("denary", pszMonitor) == 0) *target = 9;
	else if (_stricmp("duodenary", pszMonitor) == 0) *target = 11;
	else {
		char * endPtr;
		UINT u = strtoul(pszMonitor, &endPtr, 0);

		if (*pszMonitor == '\0' || *endPtr != '\0') {
			return false;
		}

		*target = u;
	}
	return true;
}

/// <summary>
/// Parses a user specified color.
/// </summary>
EXPORT_CDECL(bool) ParseColor(LPCSTR pszColor, ARGB* target) {
	// Useful information
	size_t uLength;
	StringCchLength(pszColor, MAX_LINE_LENGTH, &uLength);

	// #RGB, #ARGB, #RRGGBB, #AARRGGBB
	if (pszColor[0] == '#') {
		// Try to parse the input as a hex string
		char * endPtr;
		ARGB color = strtoul(pszColor+1, &endPtr, 16);
		if (*endPtr != '\0')
			return false;
		
		switch (uLength) {
		case 4:
		case 5:
			// Alpha
			*target = uLength == 4 ? 0xFF000000 : ((0xF000 & color) << 16 | (0xF000 & color) << 12);
			// Red
			*target |= (0xF00 & color) << 12 | (0xF00 & color) << 8;
			// Green
			*target |= (0xF0 & color) << 8 | (0xF0 & color) << 4;
			// Blue
			*target |= (0xF & color) << 4 | (0xF & color);
			return true;
		case 7:
			*target = 0xFF000000 | color;
			return true;
		case 9:
			*target = color;
			return true;
		default:
			return false;
		}
	}

	// TODO::RGB()

	// TODO::ARGB()

	// Known colors
	for (int i = 0; knownColors[i].name != NULL; i++) {
		if (_stricmp(pszColor, knownColors[i].name) == 0) {
			*target = knownColors[i].color;
			return true;
		}
	}

	// Unkown
	return false;
}

/// <summary>
/// Reads a prefixed color value from an RC file
/// </summary>
EXPORT_CDECL(ARGB) GetPrefixedRCColor(LPCSTR szPrefix, LPCSTR szOption, ARGB nDefault) {
	char szOptionName[MAX_LINE_LENGTH];
	char szColor[64];
	ARGB ret = nDefault;

	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	GetRCLine(szOptionName, szColor, sizeof(szColor), "");

	ParseColor(szColor, &ret);
	return ret;
}

/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCBool(LPCSTR szPrefix, LPCSTR szOption, bool bDefault) {
	char szOptionName[MAX_LINE_LENGTH];
	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	return GetRCBoolDef(szOptionName, bDefault) != FALSE;
}

/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCString(LPCSTR szPrefix, LPCSTR szOption, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer) {
	char szOptionName[MAX_LINE_LENGTH];
	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	return GetRCString(szOptionName, pszBuffer, pszDefault, cbBuffer) != FALSE;
}

/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCWString(LPCSTR szPrefix, LPCSTR szOption, LPWSTR pszwBuffer, LPCSTR pszDefault, UINT cbBuffer) {
	char szOptionName[MAX_LINE_LENGTH];
	bool ret;
	LPSTR szBuffer = (LPSTR)malloc(cbBuffer);
	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	ret = GetRCString(szOptionName, szBuffer, pszDefault, cbBuffer) != FALSE;
	MultiByteToWideChar(CP_ACP, 0, szBuffer, cbBuffer, pszwBuffer, cbBuffer);
	free(szBuffer);
	return ret;
}

/// <summary>
/// Reads a prefixed float value from an RC file
/// </summary>
EXPORT_CDECL(float) GetPrefixedRCFloat(LPCSTR szPrefix, LPCSTR szOption, float fDefault) {
	char szOptionName[MAX_LINE_LENGTH], szFloat[64];
	char *endPtr;

	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	GetRCString(szOptionName, szFloat, "", sizeof(szFloat));
	float f = (float)strtod(szFloat, &endPtr);

	return (*endPtr != '\0' || szFloat[0] == '\0') ? fDefault : f;
}

/// <summary>
/// Reads a prefixed float value from an RC file
/// </summary>
EXPORT_CDECL(double) GetPrefixedRCDouble(LPCSTR szPrefix, LPCSTR szOption, double dDefault) {
	char szOptionName[MAX_LINE_LENGTH], szDouble[64];
	char *endPtr;

	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	GetRCString(szOptionName, szDouble, "", sizeof(szDouble));
	double d = strtod(szDouble, &endPtr);

	return (*endPtr != '\0' || szDouble[0] == '\0') ? dDefault : d;
}

/// <summary>
/// Reads a prefixed monitor value from an RC file
/// </summary>
EXPORT_CDECL(UINT) GetPrefixedRCMonitor(LPCSTR szPrefix, LPCSTR szOption, UINT uDefault) {
	char szOptionName[MAX_LINE_LENGTH], szMonitor[64];
	UINT monitor;

	StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
	GetRCString(szOptionName, szMonitor, "", sizeof(szMonitor));

	return ParseMonitor(szMonitor, &monitor) ? monitor : uDefault;
}