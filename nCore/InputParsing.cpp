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
#include "../nShared/Math.h"
#include <dwmapi.h>


EXPORT_CDECL(bool) ParseColor(LPCSTR color, LPARGB target);


/// <summary>
/// Parses a user inputed coordinate.
/// </summary>
/// <param name="coordinate">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid coordinate.</param>
/// <param name="canBeRelative">Ttrue if the coordinate can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the coordinate can be negative.</param>
/// <returns>True if szCoordinate is a valid coordinate.</return>
EXPORT_CDECL(bool) ParseCoordinateEx(LPCSTR coordinate, LPINT target, bool /* canBeRelative */ = true, bool canBeNegative = true) {
    LPSTR endPtr;
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
EXPORT_CDECL(bool) ParseLength(LPCSTR length, LPINT target, bool /* canBeRelative */ = true, bool canBeNegative = false) {
    LPSTR endPtr;
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
    char optionName[MAX_LINE_LENGTH];
    StringCchPrintf(optionName, MAX_LINE_LENGTH, "%s%s", prefix, option);
    return LiteStep::GetRCInt(optionName, default);
}


/// <summary>
/// Parses a monitor definition
/// </summary>
EXPORT_CDECL(bool) ParseMonitor(LPCSTR monitor, LPUINT target) {
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
        LPSTR endPtr;
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
static inline bool _IsFunctionOf(LPCSTR source, LPCSTR name) {
    return _strnicmp(name, source, strlen(name)) == 0 && source[strlen(name)] == '(' &&  source[strlen(source)-1] == ')';
}


/// <summary>
/// Grabs the first maxParams parameters from the function contained in the given string.
/// </summary>
/// <returns>The number of parameters actually retrived.</returns>
static int _GetParameters(LPCSTR source, UCHAR maxParams, LPSTR dests[], size_t cchDest) {
    // *(),
    int currentParam = 0;
    int parenDepth = 0;

    LPCSTR pos = strchr(source, '(');
    LPCSTR paramStart = pos + 1;

    bool done = false;
    while (!done && currentParam < maxParams) {
        switch (*++pos)
        {
        case '\0':
            done = true;
            break;

        case '(':
            ++parenDepth;
            break;

        case ',':
            if (parenDepth == 0)
            {
                StringCchCopyN(dests[currentParam++], cchDest, paramStart, pos - paramStart);
                while (*(pos + 1) == ' ' || *(pos + 1) == '\t') ++pos;
                paramStart = pos + 1;
            }
            break;

        case ')':
            if (parenDepth == 0)
            {
                StringCchCopyN(dests[currentParam++], cchDest, paramStart, pos - paramStart);
                done = true;
            }
            else
            {
                --parenDepth;
            }
            break;
        }
    }

    return currentParam;
}


static int _GetColorDefParams(LPCSTR source, UCHAR maxParams, LPINT out) {
    char val1[8], val2[8], val3[8], val4[8];
    LPSTR params[] = { val1, val2, val3, val4 };
    LPSTR endPtr;

    int numParams = _GetParameters(source, maxParams, params, 8);
    
    for (int i = 0; i < numParams; ++i) {
        out[i] = strtoul(params[i], &endPtr, 0);

        if (*endPtr != '\0') {
            return i;
        }
    }

    return numParams;
}


static bool _GetColorAndAmount(LPCSTR source, LPARGB color, LPLONG amount) {
    char val1[MAX_LINE_LENGTH], val2[MAX_LINE_LENGTH];
    LPSTR params[] = { val1, val2 };
    LPSTR endPtr;

    if (_GetParameters(source, 2, params, MAX_LINE_LENGTH) != 2)
        return false;

    *amount = strtol(val2, &endPtr, 0);

    if (*endPtr != '\0' || !ParseColor(val1, color))
        return false;

    return true;
}


/// <summary>
/// Parses a user specified color.
/// </summary>
EXPORT_CDECL(bool) ParseColor(LPCSTR color, LPARGB target) {
    // Useful information
    size_t length;
    StringCchLength(color, MAX_LINE_LENGTH, &length);

    // This happens a lot
    if (*color == L'\0') {
        return false;
    }

    if (color[0] == '#') {
        // Try to parse the input as a hex string
        LPSTR endPtr;
        ARGB colorValue = strtoul(color + 1, &endPtr, 16);
        if (*endPtr != '\0')
            return false;
        
        switch (length) {
        case 4: // #RGB
        case 5: // #ARGB
            // Alpha
            *target = length == 4 ? 0xFF000000 : ((0xF000 & colorValue) << 16 | (0xF000 & colorValue) << 12);
            // Red
            *target |= (0xF00 & colorValue) << 12 | (0xF00 & colorValue) << 8;
            // Green
            *target |= (0xF0 & colorValue) << 8 | (0xF0 & colorValue) << 4;
            // Blue
            *target |= (0xF & colorValue) << 4 | (0xF & colorValue);
            return true;

        case 7: // #RRGGBB
            *target = 0xFF000000 | colorValue;
            return true;

        case 9: // #AARRGGBB
            *target = colorValue;
            return true;

        default:
            return false;
        }
    }
    else if (_IsFunctionOf(color, "RGB")) { // RGB(red, green, blue)
        int params[3];
        if (_GetColorDefParams(color, 3, params) != 3)
            return false;

        *target = Color::RGBToARGB(clamp(params[0], 0, 255), clamp(params[1], 0, 255), clamp(params[2], 0, 255));
        return true;
    }
    else if (_IsFunctionOf(color, "ARGB")) { // ARGB(alpha, red, green, blue)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::ARGBToARGB(clamp(params[0], 0, 255), clamp(params[1], 0, 255), clamp(params[2], 0, 255), clamp(params[3], 0, 255));
        return true;
    }
    else if (_IsFunctionOf(color, "RGBA")) { // RGBA(red, green, alpha, blue)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::ARGBToARGB(clamp(params[3], 0, 255), clamp(params[0], 0, 255), clamp(params[1], 0, 255), clamp(params[2], 0, 255));
        return true;
    }
    else if (_IsFunctionOf(color, "HSL")) { // HSL(hue, saturation, lightness)
        int params[3];
        if (_GetColorDefParams(color, 3, params) != 3)
            return false;

        *target = Color::HSLToARGB(clamp(params[0], 0, COLOR_MAX_HUE), (float)clamp(params[1], 0, COLOR_MAX_SATURATION), (float)clamp(params[2], 0, COLOR_MAX_LIGHTNESS));
        return true;
    }
    else if (_IsFunctionOf(color, "HSLA")) { // HSLA(hue, saturation, lightness, alpha)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::AHSLToARGB(clamp(params[3], 0, 255), clamp(params[0], 0, COLOR_MAX_HUE), (float)clamp(params[1], 0, COLOR_MAX_SATURATION), (float)clamp(params[2], 0, COLOR_MAX_LIGHTNESS));
        return true;
    }
    else if (_IsFunctionOf(color, "AHSL")) { // AHSL(alpha, hue, saturation, lightness)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::AHSLToARGB(clamp(params[0], 0, 255), clamp(params[1], 0, COLOR_MAX_HUE), (float)clamp(params[2], 0, COLOR_MAX_SATURATION), (float)clamp(params[3], 0, COLOR_MAX_LIGHTNESS));
        return true;
    }
    else if (_IsFunctionOf(color, "HSV")) { // HSV(hue, saturation, value)
        int params[3];
        if (_GetColorDefParams(color, 3, params) != 3)
            return false;

        *target = Color::HSVToARGB(clamp(params[0], 0, COLOR_MAX_HUE), clamp(params[1], 0, COLOR_MAX_SATURATION), clamp(params[2], 0, COLOR_MAX_VALUE));
        return true;
    }
    else if (_IsFunctionOf(color, "HSVA")) { // HSVA(hue, saturation, value, alpha)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::AHSVToARGB(clamp(params[3], 0, 255), clamp(params[0], 0, COLOR_MAX_HUE), clamp(params[1], 0, COLOR_MAX_SATURATION), clamp(params[2], 0, COLOR_MAX_VALUE));
        return true;
    }
    else if (_IsFunctionOf(color, "AHSV")) { // AHSV(alpha, hue, saturation, value)
        int params[4];
        if (_GetColorDefParams(color, 4, params) != 4)
            return false;

        *target = Color::AHSVToARGB(clamp(params[0], 0, 255), clamp(params[1], 0, COLOR_MAX_HUE), clamp(params[2], 0, COLOR_MAX_SATURATION), clamp(params[3], 0, COLOR_MAX_VALUE));
        return true;
    }
    else if (_IsFunctionOf(color, "Lighten")) { // Lighten(color, amount) -- Increases the lightness of the color by the specified amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.lightness = (UCHAR)clamp(hslColor.lightness + amount, 0, COLOR_MAX_LIGHTNESS);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "Darken")) { // Darken(color, amount) -- Decreases the lightness of the color by the specified amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.lightness = (UCHAR)clamp(hslColor.lightness - amount, 0, COLOR_MAX_LIGHTNESS);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "SetLightness")) { // SetLightness(color, amount) -- Sets the lightness of the color to the specified amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.lightness = (UCHAR)clamp(amount, 0, COLOR_MAX_LIGHTNESS);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "Saturate")) { // Saturate(color, amount) -- Increases the saturation by amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.saturation = (UCHAR)clamp(hslColor.saturation + amount, 0, COLOR_MAX_SATURATION);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "Desaturate")) { // Desaturate(color, amount) -- Decreases the saturation by amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.saturation = (UCHAR)clamp(hslColor.saturation - amount, 0, COLOR_MAX_SATURATION);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "SetSaturation")) { // SetSaturation(color, amount) -- Sets the saturation to amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);
        hslColor.saturation = (UCHAR)clamp(amount, 0, COLOR_MAX_SATURATION);

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "Fadein")) { // Fadein(color, amount) -- Increases the alpha by amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        long newAlpha = clamp((parsedColor >> 24) + amount, 0, 255);
        *target = newAlpha << 24 | parsedColor & 0xFFFFFF;
        return true;
    }
    else if (_IsFunctionOf(color, "Fadeout")) { // Fadeout(color, amount) -- Reduces the alpha by amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        long newAlpha = clamp((parsedColor >> 24) - amount, 0, 255);
        *target = newAlpha << 24 | parsedColor & 0xFFFFFF;
        return true;
    }
    else if (_IsFunctionOf(color, "SetAlpha")) { // SetAlpha(color, amount) -- Sets the alpha to the specified amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        *target = clamp(amount, 0, 255) << 24 | parsedColor & 0xFFFFFF;
        return true;
    }
    else if (_IsFunctionOf(color, "Spin")) { // Spin(color, amount) -- Spins the hue of the color by amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);

        hslColor.hue += amount;
        hslColor.hue %= 360;
        if (hslColor.hue < 0)
            hslColor.hue += 360;

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "SetHue")) { // SetHue(color, amount) -- Sets the hue to the specified amount.
        ARGB parsedColor;
        long amount;
        if (!_GetColorAndAmount(color, &parsedColor, &amount))
            return false;

        AHSL hslColor = Color::ARGBToAHSL(parsedColor);

        hslColor.hue = amount;
        hslColor.hue %= 360;
        if (hslColor.hue < 0)
            hslColor.hue += 360;

        *target = Color::AHSLToARGB(hslColor);
        return true;
    }
    else if (_IsFunctionOf(color, "Mix")) { // Mix(color1, color2, weight) -- Mixes color1 and color2.
        char val1[MAX_LINE_LENGTH], val2[MAX_LINE_LENGTH], val3[MAX_LINE_LENGTH];
        LPSTR params[] = { val1, val2, val3 };
        LPSTR endPtr;

        ARGB color1, color2;

        if (_GetParameters(color, 3, params, MAX_LINE_LENGTH) != 3)
            return false;

        // strtof does not seem to set endptr properly
        float weight = (float)strtod(val3, &endPtr);

        if (*endPtr != '\0' || !ParseColor(val1, &color1) || !ParseColor(val2, &color2))
            return false;

        *target = Color::Mix(color1, color2, weight);

        return true;
    }
    else if (_stricmp(color, "DWMColor") == 0) {
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
