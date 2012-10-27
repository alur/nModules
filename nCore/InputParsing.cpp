/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InputParsing.cpp
 *  The nModules Project
 *
 *  Utility functions for parsing input.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nShared/Macros.h"
#include "../nShared/Color.h"
#include <dwmapi.h>


/// <summary>
/// Parses a user inputed coordinate.
/// </summary>
/// <param name="coordinate">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid coordinate.</param>
/// <param name="canBeRelative">Ttrue if the coordinate can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the coordinate can be negative.</param>
/// <returns>True if szCoordinate is a valid coordinate.</return>
EXPORT_CDECL(bool) ParseCoordinateEx(LPCSTR coordinate, int * target, bool /* canBeRelative */ = true, bool canBeNegative = true) {
    char * endPtr;
    int i = strtol(coordinate, &endPtr, 0);

    if (*coordinate == '\0' || *endPtr != '\0' || (!canBeNegative && i < 0)) {
        return false;
    }

    *target = i;

    return true;
}


/// <summary>
/// Parses a user inputed length.
/// </summary>
/// <param name="length">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid length.</param>
/// <param name="canBeRelative">Ttrue if the length can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the length can be negative.</param>
/// <returns>True if szLength is a valid length.</return>
EXPORT_CDECL(bool) ParseLength(LPCSTR length, int * target, bool /* canBeRelative */ = true, bool canBeNegative = false) {
    char * endPtr;
    int i = strtol(length, &endPtr, 0);

    if (*length == '\0' || *endPtr != '\0' || (!canBeNegative && i < 0)) {
        return false;
    }

    *target = i;

    return true;
}


/// <summary>
/// Reads a prefixed integer value from an RC file
/// </summary>
EXPORT_CDECL(int) GetPrefixedRCInt(LPCSTR prefix, LPCSTR option, int default) {
    char szOptionName[MAX_LINE_LENGTH];
    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", prefix, option);
    return LiteStep::GetRCInt(szOptionName, default);
}


/// <summary>
/// Parses a monitor definition
/// </summary>
EXPORT_CDECL(bool) ParseMonitor(LPCSTR monitor, UINT * target) {
    if (_stricmp("primary", monitor) == 0) *target = 0;
    else if (_stricmp("secondary", monitor) == 0) *target = 1;
    else if (_stricmp("tertiary", monitor) == 0) *target = 2;
    else if (_stricmp("quaternary", monitor) == 0) *target = 3;
    else if (_stricmp("quinary", monitor) == 0) *target = 4;
    else if (_stricmp("senary", monitor) == 0) *target = 5;
    else if (_stricmp("septenary", monitor) == 0) *target = 6;
    else if (_stricmp("octonary", monitor) == 0) *target = 7;
    else if (_stricmp("nonary", monitor) == 0) *target = 8;
    else if (_stricmp("denary", monitor) == 0) *target = 9;
    else if (_stricmp("duodenary", monitor) == 0) *target = 11;
    else if (_stricmp("all", monitor) == 0) *target = 0xFFFFFFFF;
    else {
        char * endPtr;
        UINT u = strtoul(monitor, &endPtr, 0);

        if (*monitor == '\0' || *endPtr != '\0') {
            return false;
        }

        *target = u;
    }
    return true;
}


/// <summary>
/// Utility function used by ParseColor
/// </summary>
inline bool _IsFunctionOf(LPCSTR source, LPCSTR name) {
    return _strnicmp(name, source, strlen(name)) == 0 && source[strlen(name)] == '(' &&  source[strlen(source)-1] == ')';
}


/// <summary>
/// Parses a user specified color.
/// </summary>
EXPORT_CDECL(bool) ParseColor(LPCSTR color, ARGB* target) {
    // Useful information
    size_t length;
    StringCchLength(color, MAX_LINE_LENGTH, &length);

    // #RGB, #ARGB, #RRGGBB, #AARRGGBB
    if (color[0] == '#') {
        // Try to parse the input as a hex string
        char * endPtr;
        ARGB colorValue = strtoul(color+1, &endPtr, 16);
        if (*endPtr != '\0')
            return false;
        
        switch (length) {
        case 4:
        case 5:
            // Alpha
            *target = length == 4 ? 0xFF000000 : ((0xF000 & colorValue) << 16 | (0xF000 & colorValue) << 12);
            // Red
            *target |= (0xF00 & colorValue) << 12 | (0xF00 & colorValue) << 8;
            // Green
            *target |= (0xF0 & colorValue) << 8 | (0xF0 & colorValue) << 4;
            // Blue
            *target |= (0xF & colorValue) << 4 | (0xF & colorValue);
            return true;

        case 7:
            *target = 0xFF000000 | colorValue;
            return true;

        case 9:
            *target = colorValue;
            return true;

        default:
            return false;
        }
    }

    // TODO::RGB(), ARGB(), RGBA(), HSL(), AHSL(), HSLA()
    if (_IsFunctionOf(color, "RGB")) {
    }

    // TODO::Lighten, darken, saturate, desaturate, fadein, fadeout, spin, mix
    if (_IsFunctionOf(color, "lighten")) {
    }

    if (_IsFunctionOf(color, "darken")) {
    }

    //
    if (_stricmp(color, "DWMColor") == 0) {
        BOOL b;
        DwmGetColorizationColor(target, &b);
        return true;
    }

    // Known colors
    Color::KnownColor* knownColors = Color::GetKnownColors();
    for (int i = 0; knownColors[i].name != NULL; i++) {
        if (_stricmp(color, knownColors[i].name) == 0) {
            *target = knownColors[i].color;
            return true;
        }
    }

    // Unknown
    return false;
}


/// <summary>
/// String -> Bool
/// </summary>
EXPORT_CDECL(bool) ParseBool(LPCSTR boolean) {
    return lstrcmpi(boolean, "off") && lstrcmpi(boolean, "false") && lstrcmpi(boolean, "no");
}


/// <summary>
/// Reads a prefixed color value from an RC file
/// </summary>
EXPORT_CDECL(ARGB) GetPrefixedRCColor(LPCSTR prefix, LPCSTR option, ARGB default) {
    char optionName[MAX_LINE_LENGTH];
    char color[MAX_LINE_LENGTH];
    ARGB ret = default;

    StringCchPrintf(optionName, MAX_LINE_LENGTH, "%s%s", prefix, option);
    LiteStep::GetRCLine(optionName, color, sizeof(color), "");

    ParseColor(color, &ret);
    return ret;
}


/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCBool(LPCSTR prefix, LPCSTR option, bool default) {
    char optionName[MAX_LINE_LENGTH];
    StringCchPrintf(optionName, MAX_LINE_LENGTH, "%s%s", prefix, option);
    return LiteStep::GetRCBoolDef(optionName, default) != FALSE;
}


/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCString(LPCSTR szPrefix, LPCSTR szOption, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer) {
    char szOptionName[MAX_LINE_LENGTH];
    LPCSTR def = pszDefault == pszBuffer ? _strdup(pszDefault) : pszDefault; // LiteStep will null the first character of the buffer.
    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
    bool ret = LiteStep::GetRCString(szOptionName, pszBuffer, def, cbBuffer) != FALSE;
    if (def != pszDefault) {
        free((LPVOID)def);
    }
    return ret;
}


/// <summary>
/// Reads a prefixed bool value from an RC file
/// </summary>
EXPORT_CDECL(bool) GetPrefixedRCWString(LPCSTR szPrefix, LPCSTR szOption, LPWSTR pszwBuffer, LPCSTR pszDefault, UINT cbBuffer) {
    char optionName[MAX_LINE_LENGTH];
    bool ret;
    LPSTR buffer = (LPSTR)malloc(cbBuffer/2);
    StringCchPrintf(optionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
    ret = LiteStep::GetRCString(optionName, buffer, pszDefault, cbBuffer/2) != FALSE;
    MultiByteToWideChar(CP_ACP, 0, buffer, cbBuffer/2, pszwBuffer, cbBuffer);
    free(buffer);
    return ret;
}


/// <summary>
/// Reads a prefixed float value from an RC file
/// </summary>
EXPORT_CDECL(float) GetPrefixedRCFloat(LPCSTR szPrefix, LPCSTR szOption, float fDefault) {
    char szOptionName[MAX_LINE_LENGTH], szFloat[64];
    char *endPtr;

    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", szPrefix, szOption);
    LiteStep::GetRCString(szOptionName, szFloat, "", sizeof(szFloat));
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
    LiteStep::GetRCString(szOptionName, szDouble, "", sizeof(szDouble));
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
    LiteStep::GetRCString(szOptionName, szMonitor, "", sizeof(szMonitor));

    return ParseMonitor(szMonitor, &monitor) ? monitor : uDefault;
}
