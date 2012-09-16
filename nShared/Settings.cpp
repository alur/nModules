/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.cpp                                                  August, 2012
 *  The nModules Project
 *
 *  Manages RC settings with a certain prefix.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "Settings.hpp"
#include "Error.h"
#include "../nCoreCom/Core.h"

using namespace nCore::InputParsing;


/// <summary>
/// Initalizes a new Settings class.
/// </summary>
/// <param name="pszPrefix">The RC prefix to use.</param>
Settings::Settings(LPCSTR pszPrefix) {
    m_pszPrefix = _strdup(pszPrefix);
    m_pGroup = GetGroup(NULL);
}


/// <summary>
/// Initalizes a new Settings class, due to the precense of a Group setting in another class.
/// </summary>
/// <param name="pszPrefix">The RC prefix to use.</param>
/// <param name="pszPrev">A list of previous group names.</param>
Settings::Settings(LPCSTR pszPrefix, LPCSTR pszPrev[]) {
    m_pszPrefix = _strdup(pszPrefix);
    m_pGroup = GetGroup(pszPrev);
}


/// <summary>
/// Deallocates resources used by the Settings class.
/// </summary>
Settings::~Settings() {
    free((LPVOID)m_pszPrefix);
    if (m_pGroup) {
        delete m_pGroup;
    }
}


/// <summary>
/// Gets the value we should use for m_pGroup.
/// </summary>
/// <param name="pszPrev">A list of previous group names.</param>
/// <returns>The value we should use for m_pGroup.</returns>
Settings* Settings::GetGroup(LPCSTR pszPrev[]) {
    char szBuf[MAX_LINE_LENGTH];
    GetPrefixedRCString(m_pszPrefix, "Group", szBuf, "", sizeof(szBuf));
    
    // If there is no group
    if (szBuf[0] == '\0') {
        // We need to free pszPrev at this point.
        if (pszPrev != NULL) {
            free((LPVOID)pszPrev);
        }
        
        return NULL;
    }
    
    // Avoid circular definitions
    int i = 0;
    if (pszPrev != NULL) {
        for (; pszPrev[i] != NULL; i++) {
            if (strcmp(pszPrev[i], szBuf) == 0) {
                // We found a circle :/
                
                // Show an error message
                char szMsg[MAX_LINE_LENGTH];
                StringCchCopy(szMsg, sizeof(szMsg), "Circular group definition!\n");
                
                // A -> B -> C -> ... -> C
                for (int j = 0; pszPrev[j] != NULL; j++) {
                    StringCchCat(szMsg, sizeof(szMsg), pszPrev[j]);
                    StringCchCat(szMsg, sizeof(szMsg), " -> ");
                }
                StringCchCat(szMsg, sizeof(szMsg), szBuf);
                
                ErrorMessage(E_LVL_ERROR, szMsg);
                
                // And break out of the chain
                free((LPVOID)pszPrev);
                return NULL;
            }
        }
    }
    
    // Allocate space for storing the prefix of this setting
    pszPrev = (LPCSTR*)realloc(pszPrev, (i+2)*sizeof(LPCSTR));
    pszPrev[i] = m_pszPrefix;
    pszPrev[i+1] = NULL;

    return new Settings(szBuf, pszPrev);
}


/// <summary>
/// Get's a color from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="defColor">The default color to use, if the setting is invalid or unspecified.</param>
/// <returns>The color.</returns>
ARGB Settings::GetColor(LPCSTR pszSetting, ARGB defColor) {
    return GetPrefixedRCColor(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetColor(pszSetting, defColor) : defColor);
}


/// <summary>
/// Set's a prefixed RC value to a particular color.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="colorValue">The value to set the setting to.</param>
void Settings::SetColor(LPCSTR pszSetting, ARGB colorValue) {
    char szString[32];
    StringCchPrintf(szString, sizeof(szString), "%x", colorValue);
    SetString(pszSetting, szString);
}


/// <summary>
/// Get's a string from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszDest">Where the string should be read to.</param>
/// <param name="cchDest">The maximum number of characters to write to pszDest.</param>
/// <param name="pszDefault">The default string, used if the RC value is unspecified.</param>
/// <returns>False if the length of the RC value is > cchDest. True otherwise.</returns>
bool Settings::GetString(LPCSTR pszSetting, LPSTR pszDest, UINT cchDest, LPCSTR pszDefault) {
    return GetPrefixedRCString(m_pszPrefix, pszSetting, pszDest, pszDefault, cchDest);
}


/// <summary>
/// Get's a string from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszwDest">Where the string should be read to.</param>
/// <param name="cchDest">The maximum number of characters to write to pszDest.</param>
/// <param name="pszDefault">The default string, used if the RC value is unspecified.</param>
/// <returns>False if the length of the RC value is > cchDest. True otherwise.</returns>
bool Settings::GetString(LPCSTR pszSetting, LPWSTR pszwDest, UINT cchDest, LPCSTR pszDefault) {
    return GetPrefixedRCWString(m_pszPrefix, pszSetting, pszwDest, pszDefault, cchDest);
}


/// <summary>
/// Set's a prefixed RC value to a particular string.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszValue">The value to set the setting to.</param>
void Settings::SetString(LPCSTR pszSetting, LPCSTR pszValue) {
    char szOptionName[MAX_LINE_LENGTH];
    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", m_pszPrefix, pszSetting);
    LSSetVariable(szOptionName, pszValue);
}


/// <summary>
/// Get's an integer from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="iDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The integer.</returns>
int Settings::GetInt(LPCSTR pszSetting, int iDefault) {
    return GetPrefixedRCInt(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetInt(pszSetting, iDefault) : iDefault);
}


/// <summary>
/// Set's a prefixed RC value to a particular integer.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="iValue">The value to set the setting to.</param>
void Settings::SetInt(LPCSTR pszSetting, int iValue) {
    char szString[10];
    _itoa_s(iValue, szString, sizeof(szString), 10);
    SetString(pszSetting, szString);
}


/// <summary>
/// Get's a float from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="fDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The float.</returns>
float Settings::GetFloat(LPCSTR pszSetting, float fDefault) {
    return GetPrefixedRCFloat(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetFloat(pszSetting, fDefault) : fDefault);
}


/// <summary>
/// Set's a prefixed RC value to a particular float.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="fValue">The value to set the setting to.</param>
void Settings::SetFloat(LPCSTR pszSetting, float fValue) {
    char szString[32];
    StringCchPrintf(szString, sizeof(szString), "%.30f", fValue);
    SetString(pszSetting, szString);
}


/// <summary>
/// Get's a double from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="dDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The double.</returns>
double Settings::GetDouble(LPCSTR pszSetting, double dDefault) {
    return GetPrefixedRCDouble(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetDouble(pszSetting, dDefault) : dDefault);
}


/// <summary>
/// Set's a prefixed RC value to a particular double.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="dValue">The value to set the setting to.</param>
void Settings::SetDouble(LPCSTR pszSetting, double dValue) {
    char szString[32];
    StringCchPrintf(szString, sizeof(szString), "%.30f", dValue);
    SetString(pszSetting, szString);
}


/// <summary>
/// Get's a boolean from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="bDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The boolean.</returns>
bool Settings::GetBool(LPCSTR pszSetting, bool bDefault) {
    return GetPrefixedRCBool(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetBool(pszSetting, bDefault) : bDefault);
}


/// <summary>
/// Set's a prefixed RC value to a particular boolean.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="bValue">The value to set the setting to.</param>
void Settings::SetBool(LPCSTR pszSetting, bool bValue) {
    SetString(pszSetting, bValue ? "True" : "False");
}


/// <summary>
/// Get's a Monitor from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="uDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The monitor.</returns>
UINT Settings::GetMonitor(LPCSTR pszSetting, UINT uDefault) {
    return GetPrefixedRCMonitor(m_pszPrefix, pszSetting, m_pGroup != NULL ? m_pGroup->GetMonitor(pszSetting, uDefault) : uDefault);
}


/// <summary>
/// Set's a prefixed RC value to a particular monitor.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="uValue">The value to set the setting to.</param>
void Settings::SetMonitor(LPCSTR pszSetting, UINT uValue) {
    char szString[10];
    StringCchPrintf(szString, sizeof(szString), "%u", uValue);
    SetString(pszSetting, szString);
}


/// <summary>
/// Reads an X, Y, Width, Height series of settings into a RECT.
/// </summary>
/// <param name="pszX">The key for the X value.</param>
/// <param name="pszY">The key for the Y value.</param>
/// <param name="pszW">The key for the width value.</param>
/// <param name="pszH">The key for the height value.</param>
/// <param name="pDest">The destination RECT.</param>
/// <param name="pDefault">The default x/y/width/height, if not specified.</param>
void Settings::GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, LPRECT pDefault) {
	GetRectFromXYWH(pszX, pszY, pszW, pszH, pDest, pDefault->left, pDefault->top, pDefault->right - pDefault->left, pDefault->bottom - pDefault->top);
}


/// <summary>
/// Reads an X, Y, Width, Height series of settings into a RECT.
/// </summary>
/// <param name="pszX">The key for the X value.</param>
/// <param name="pszY">The key for the Y value.</param>
/// <param name="pszW">The key for the width value.</param>
/// <param name="pszH">The key for the height value.</param>
/// <param name="pDest">The destination RECT.</param>
/// <param name="defX">The default X value, if not specified.</param>
/// <param name="defY">The default Y value, if not specified.</param>
/// <param name="defW">The default width, if not specified.</param>
/// <param name="defH">The default height, if not specified.</param>
void Settings::GetRectFromXYWH(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pDest, int defX, int defY, int defW, int defH) {
	pDest->left = GetInt(pszX, defX);
	pDest->top = GetInt(pszY, defY);
	pDest->right = pDest->left + GetInt(pszW, defW);
	pDest->bottom = pDest->top + GetInt(pszH, defH);
}


/// <summary>
/// Sets an X, Y, Width, Height series of settings from a RECT.
/// </summary>
/// <param name="pszX">The key for the X value.</param>
/// <param name="pszY">The key for the Y value.</param>
/// <param name="pszW">The key for the width value.</param>
/// <param name="pszH">The key for the height value.</param>
/// <param name="pValue">The source RECT.</param>
void Settings::SetXYWHFromRect(LPCSTR pszX, LPCSTR pszY, LPCSTR pszW, LPCSTR pszH, LPRECT pValue) {
	SetInt(pszX, pValue->left);
	SetInt(pszY, pValue->top);
	SetInt(pszW, pValue->right - pValue->left);
	SetInt(pszH, pValue->bottom - pValue->top);
}


/// <summary>
/// Reads a series of values into a RECT structure.
/// </summary>
/// <param name="pszLeft">The key for the left value.</param>
/// <param name="pszTop">The key for the top value.</param>
/// <param name="pszRight">The key for the right value.</param>
/// <param name="pszBottom">The key for the bottom value.</param>
/// <param name="pDest">The destination RECT.</param>
/// <param name="pDefault">The default values.</param>
void Settings::GetOffsetRect(LPCSTR pszLeft, LPCSTR pszTop, LPCSTR pszRight, LPCSTR pszBottom, LPRECT pDest, LPRECT pDefault) {
	GetOffsetRect(pszLeft, pszTop, pszRight, pszBottom, pDest, pDefault->left, pDefault->top, pDefault->right, pDefault->bottom);
}


/// <summary>
/// Reads a series of values into a RECT structure.
/// </summary>
/// <param name="pszLeft">The key for the left value.</param>
/// <param name="pszTop">The key for the top value.</param>
/// <param name="pszRight">The key for the right value.</param>
/// <param name="pszBottom">The key for the bottom value.</param>
/// <param name="pDest">The destination RECT.</param>
/// <param name="defLeft">The default left value, if not specified.</param>
/// <param name="defTop">The default top value, if not specified.</param>
/// <param name="defRight">The default right, if not specified.</param>
/// <param name="defBottom">The default bottom, if not specified.</param>
void Settings::GetOffsetRect(LPCSTR pszLeft, LPCSTR pszTop, LPCSTR pszRight, LPCSTR pszBottom, LPRECT pDest, int defLeft, int defTop, int defRight, int defBottom) {
	pDest->left = GetInt(pszLeft, defLeft);
	pDest->top = GetInt(pszTop, defTop);
	pDest->right = GetInt(pszRight, defRight);
	pDest->bottom = GetInt(pszBottom, defBottom);
}
