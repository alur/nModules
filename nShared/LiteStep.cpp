/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  LiteStep.cpp
 *  The nModules Project
 *
 *  Confines LSAPI routines to a namespace, and provides extra functions for
 *  interaction with LiteStep.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include <strsafe.h>

using std::function;


/// <summary>
/// Retrives a bool with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
bool LiteStep::GetPrefixedRCBool(LPCTSTR prefix, LPCTSTR keyName, bool defaultValue)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCBoolDef(prefixedKey, defaultValue ? TRUE : FALSE) != FALSE;
}


/// <summary>
/// Retrives a double with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
double LiteStep::GetPrefixedRCDouble(LPCTSTR prefix, LPCTSTR keyName, double defaultValue)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCDouble(prefixedKey, defaultValue);
}


/// <summary>
/// Retrives a color with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
ARGB LiteStep::GetPrefixedRCColor(LPCTSTR prefix, LPCTSTR keyName, ARGB defaultValue)
{
    TCHAR colorString[MAX_LINE_LENGTH];
    GetPrefixedRCLine(prefix, keyName, colorString, nullptr, _countof(colorString));
    return ParseColor(colorString, defaultValue);
}


/// <summary>
/// Retrives a float with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
float LiteStep::GetPrefixedRCFloat(LPCTSTR prefix, LPCTSTR keyName, float defaultValue)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCFloat(prefixedKey, defaultValue);
}


/// <summary>
/// Retrives an integer with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
int LiteStep::GetPrefixedRCInt(LPCTSTR prefix, LPCTSTR keyName, int defaultValue)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCInt(prefixedKey, defaultValue);
}


/// <summary>
/// Retrives a 64-bit integer with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
__int64 LiteStep::GetPrefixedRCInt64(LPCTSTR prefix, LPCTSTR keyName, __int64 defaultValue)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCInt64(prefixedKey, defaultValue);
}


/// <summary>
/// Retrives a line with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="buffer">Buffer that receives the line.</param>
/// <param name="defaultValue">Default value, copied to the buffer if the key is not specified.</param>
/// <param name="cchBuffer">Size of the buffer that receives the line, in characters.</param>
/// <returns>True if the key was found in the configuration.</returns>
bool LiteStep::GetPrefixedRCLine(LPCTSTR prefix, LPCTSTR keyName, LPTSTR buffer, LPCTSTR defaultValue, size_t cchBuffer)
{
    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCLine(prefixedKey, buffer, (UINT)cchBuffer, defaultValue) != FALSE;
}


/// <summary>
/// Retrives a monitor with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="defaultValue">Default value, returned if the key is not specified.</param>
UINT LiteStep::GetPrefixedRCMonitor(LPCTSTR prefix, LPCTSTR keyName, UINT defaultValue)
{
    TCHAR monitorString[32];
    GetPrefixedRCString(prefix, keyName, monitorString, nullptr, _countof(monitorString));
    return ParseMonitor(monitorString, defaultValue);
}


/// <summary>
/// Retrives a string with a particular prefix from the LiteStep configuration.
/// </summary>
/// <param name="prefix">The prefix of the value to get.</param>
/// <param name="keyName">Key of the value to get.</param>
/// <param name="buffer">Buffer that receives the string.</param>
/// <param name="defaultValue">Default value, copied to the buffer if the key is not specified.</param>
/// <param name="cchBuffer">Size of the buffer that receives the string, in characters.</param>
/// <returns>True if the key was found in the configuration.</returns>
bool LiteStep::GetPrefixedRCString(LPCTSTR prefix, LPCTSTR keyName, LPTSTR buffer, LPCTSTR defaultValue, size_t cchBuffer)
{
    // This is bad, since the first thing GetRCString does is to set *buffer = '\0'
    assert(buffer != defaultValue);

    TCHAR prefixedKey[MAX_RCCOMMAND];
    StringCchPrintf(prefixedKey, _countof(prefixedKey), _T("%s%s"), prefix, keyName);
    return GetRCString(prefixedKey, buffer, defaultValue, (UINT)cchBuffer) != FALSE;
}


/// <summary>
/// Iterates over all lines with the specified key name.
/// </summary>
/// <param name="keyName">Key of the lines to iterate over.</param>
/// <param name="callback">Callback function to be called for each line.</param>
void LiteStep::IterateOverLines(LPCTSTR keyName, function<void(LPCTSTR line)> callback)
{
    TCHAR line[MAX_LINE_LENGTH];
    LPCTSTR callbackLine = line + _tcslen(keyName) + 1;
    LPVOID f = LCOpen(nullptr);
    while (LCReadNextConfig(f, keyName, line, _countof(line)))
    {
        callback(callbackLine);
    }
    LCClose(f);
}


/// <summary>
/// Iterates over all tokens in the specified line.
/// </summary>
/// <param name="line">The line containing tokens.</param>
/// <param name="callback">Callback function to be called for each token.</param>
void LiteStep::IterateOverTokens(LPCTSTR line, function<void (LPCTSTR token)> callback)
{
    TCHAR token[MAX_LINE_LENGTH];
    while (GetToken(line, token, &line, false))
    {
        callback(token);
    }
}


/// <summary>
/// Iterates over all tokens in all lines with the specified key name.
/// </summary>
/// <param name="keyName">Key of the lines to iterate over.</param>
/// <param name="callback">Callback function to be called for each token.</param>
void LiteStep::IterateOverLineTokens(LPCTSTR keyName, function<void (LPCTSTR token)> callback)
{
    IterateOverLines(keyName, [callback] (LPCTSTR line) -> void
    {
        IterateOverTokens(line, callback);
    });
}


/// <summary>
/// Parses a boolean from a configuration string.
/// </summary>
/// <param name="boolString">The string to parse.</param>
bool LiteStep::ParseBool(LPCTSTR boolString)
{
    return _tcsicmp(_T("off"), boolString) != 0 && _tcsicmp(_T("false"), boolString) != 0 && _tcsicmp(_T("no"), boolString) != 0;
}


/// <summary>
/// Parses a color from a configuration string.
/// </summary>
/// <param name="colorString">The string to parse.</param>
/// <param name="defaultValue">The default color, returned if the string is invalid.</param>
ARGB LiteStep::ParseColor(LPCTSTR colorString, ARGB defaultValue)
{
    return Color::Parse(colorString, defaultValue);
}


/// <summary>
/// Parses a coordinate from a configuration string.
/// </summary>
/// <param name="coordinateString">The string to parse.</param>
/// <param name="defaultValue">The default coordinate, returned if the string is invalid.</param>
Coordinate LiteStep::ParseCoordinate(LPCTSTR coordinateString, Coordinate defaultValue)
{
    if (coordinateString == nullptr)
    {
        return defaultValue;
    }

    // Valid formats
    // x+%y, %y+x, x-%y, %y-x, %x, x
    if (*coordinateString == _T('%'))
    {
    }
    else
    {
    }

    return defaultValue;
}


/// <summary>
/// Parses a length from a configuration string.
/// </summary>
/// <param name="lengthString">The string to parse.</param>
/// <param name="defaultValue">The default coordinate, returned if the string is invalid.</param>
Length ParseLength(LPCTSTR lengthString, Length defaultValue)
{
    if (lengthString == nullptr)
    {
        return defaultValue;
    }

    // Valid formats
    // x+%y, %y+x, x-%y, %y-x, %x, x
    if (*lengthString == _T('%'))
    {
    }
    else
    {
    }

    return defaultValue;
}


/// <summary>
/// Parses a monitor ID from a monitor configuration string.
/// </summary>
/// <param name="monitorString">The string to parse.</param>
/// <param name="defaultValue">Default monitor, returned if the string is not a valid monitor.</param>
UINT LiteStep::ParseMonitor(LPCTSTR monitorString, UINT defaultValue)
{
    const static struct
    {
        LPCTSTR name;
        UINT value;
    } monitorMap[] =
    {
        { _T("primary"),       0 },
        { _T("secondary"),     1 },
        { _T("tertiary"),      2 },
        { _T("quaternary"),    3 },
        { _T("quinary"),       4 },
        { _T("senary"),        5 },
        { _T("septenary"),     6 },
        { _T("octonary"),      7 },
        { _T("nonary"),        8 },
        { _T("denary"),        9 },
        { _T("duodenary"),    11 },
        { _T("all"),  0xFFFFFFFF }
    };

    if (monitorString == nullptr)
    {
        return defaultValue;
    }

    // First check if the string is a named value
    for (auto item : monitorMap)
    {
        if (_tcsicmp(item.name, monitorString) == 0)
        {
            return item.value;
        }
    }

    // Try to parse the string as an integer
    LPTSTR endPtr;
    UINT monitor = _tcstoul(monitorString, &endPtr, 0);
    if (*monitorString != _T('\0') && *endPtr == _T('\0'))
    {
        return monitor;
    }

    // Fall back to the default value
    return defaultValue;
}


/// <summary>
/// Parses a user inputed coordinate.
/// </summary>
/// <param name="coordinate">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid coordinate.</param>
/// <returns>True if szCoordinate is a valid coordinate.</return>
bool LiteStep::ParseCoordinate(LPCWSTR coordinate, LPINT target) {
    LPWSTR endPtr;
    int i = wcstol(coordinate, &endPtr, 0);

    if (*coordinate == L'\0' || *endPtr != L'\0') {
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
/// <returns>True if szLength is a valid length.</return>
bool LiteStep::ParseLength(LPCWSTR length, LPINT target) {
    LPWSTR endPtr;
    int i = wcstol(length, &endPtr, 0);

    if (*length == L'\0' || *endPtr != L'\0') {
        return false;
    }

    *target = i;

    return true;
}
