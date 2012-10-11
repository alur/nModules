/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Settings.cpp
 *  The nModules Project
 *
 *  Manages RC settings with a certain prefix.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "Settings.hpp"
#include "Error.h"
#include "../nCoreCom/Core.h"
#include "../nShared/Macros.h"

using namespace nCore::InputParsing;


/// <summary>
/// Initalizes a new Settings class.
/// </summary>
/// <param name="prefix">The RC prefix to use.</param>
Settings::Settings(LPCSTR prefix) {
    this->prefix = _strdup(prefix);
    this->group = GreateGroup(NULL);
}


/// <summary>
/// Initalizes a new Settings class, due to the precense of a Group setting in another class.
/// </summary>
/// <param name="prefix">The RC prefix to use.</param>
/// <param name="previous">A list of previous group names.</param>
Settings::Settings(LPCSTR prefix, LPCSTR previous[]) {
    this->prefix = _strdup(prefix);
    this->group = GreateGroup(previous);
}


/// <summary>
/// Deallocates resources used by the Settings class.
/// </summary>
Settings::~Settings() {
    free((LPVOID)this->prefix);
    SAFEDELETE(this->group);
}


/// <summary>
/// Creates a child of this Settings*. If you have a Settings with the prefix of Label, and you want
/// a related setting LabelIcon, you should call ->GetChild("Icon").
/// </summary>
Settings* Settings::CreateChild(LPCSTR prefix) {
    Settings *head, *thisTail, *newTail;
    CHAR newPrefix[MAX_LINE_LENGTH];
    
    StringCchPrintf(newPrefix, sizeof(newPrefix), "%s%s", this->prefix, prefix);
    head = new Settings(newPrefix);
    thisTail = this;
    newTail = head;
    while (thisTail->group != NULL) {
        thisTail = thisTail->group;
        StringCchPrintf(newPrefix, sizeof(newPrefix), "%s%s", thisTail->prefix, prefix);        
        while (newTail->group != NULL) {
            newTail = newTail->group;
        }
        newTail->group = new Settings(newPrefix);
    }

    return head;
}


/// <summary>
/// Gets the value we should use for m_pGroup.
/// </summary>
/// <param name="pszPrev">A list of previous group names.</param>
/// <returns>The value we should use for m_pGroup.</returns>
Settings* Settings::GreateGroup(LPCSTR pszPrev[]) {
    char szBuf[MAX_LINE_LENGTH];
    GetPrefixedRCString(this->prefix, "Group", szBuf, "", sizeof(szBuf));
    
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
    pszPrev[i] = this->prefix;
    pszPrev[i+1] = NULL;

    return new Settings(szBuf, pszPrev);
}


/// <summary>
/// Appends the specified prefix to the end of the group list. Essentially, lets these
/// settings fall back to that group as a default.
/// </summary>
void Settings::AppendGroup(LPCSTR prefix) {
    Settings* tail;
    for (tail = this; tail->group != NULL; tail = tail->group);
    tail->group = new Settings(prefix);
}


/// <summary>
/// Get's a color from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="defColor">The default color to use, if the setting is invalid or unspecified.</param>
/// <returns>The color.</returns>
ARGB Settings::GetColor(LPCSTR pszSetting, ARGB defColor) {
    return GetPrefixedRCColor(this->prefix, pszSetting, this->group != NULL ? this->group->GetColor(pszSetting, defColor) : defColor);
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
    if (this->group != NULL) {
        this->group->GetString(pszSetting, pszDest, cchDest, pszDefault);
        return GetPrefixedRCString(this->prefix, pszSetting, pszDest, pszDest, cchDest);
    }
    return GetPrefixedRCString(this->prefix, pszSetting, pszDest, pszDefault, cchDest);
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
    bool ret = true;
    if (this->group != NULL) {
        LPSTR def = (LPSTR)malloc(cchDest);
        ret &= this->group->GetString(pszSetting, def, cchDest, pszDefault);
        ret &= GetPrefixedRCWString(this->prefix, pszSetting, pszwDest, def, cchDest);
        free((LPVOID)def);
        return ret;
    }
    return GetPrefixedRCWString(this->prefix, pszSetting, pszwDest, pszDefault, cchDest);
}


/// <summary>
/// Get's a string from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszwDest">Where the string should be read to.</param>
/// <param name="cchDest">The maximum number of characters to write to pszDest.</param>
/// <param name="pszDefault">The default string, used if the RC value is unspecified.</param>
/// <returns>False if the length of the RC value is > cchDest. True otherwise.</returns>
bool Settings::GetString(LPCSTR pszSetting, LPWSTR pszwDest, UINT cchDest, LPCWSTR pszDefault) {
    size_t size = wcslen(pszDefault);
    LPSTR multiByte = (LPSTR)malloc(size+1);
    size_t numConverted;
    bool ret;

    if (wcstombs_s(&numConverted, multiByte, size+1, pszDefault, size) == 0) {
        ret = GetString(pszSetting, pszwDest, cchDest, multiByte);
    }
    else {
        ret = GetString(pszSetting, pszwDest, cchDest, "");
    }

    free(multiByte);

    return ret;
}


/// <summary>
/// Set's a prefixed RC value to a particular string.
/// </summary>
/// <param name="pszSetting">The RC setting.</param>
/// <param name="pszValue">The value to set the setting to.</param>
void Settings::SetString(LPCSTR pszSetting, LPCSTR pszValue) {
    char szOptionName[MAX_LINE_LENGTH];
    StringCchPrintf(szOptionName, MAX_LINE_LENGTH, "%s%s", this->prefix, pszSetting);
    LSSetVariable(szOptionName, pszValue);
}


/// <summary>
/// Get's an integer from a prefixed RC value.
/// </summary>
/// <param name="pszSetting">The RC setting to parse.</param>
/// <param name="iDefault">The default value to use, if the setting is invalid or unspecified.</param>
/// <returns>The integer.</returns>
int Settings::GetInt(LPCSTR pszSetting, int iDefault) {
    return GetPrefixedRCInt(this->prefix, pszSetting, this->group != NULL ? this->group->GetInt(pszSetting, iDefault) : iDefault);
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
    return GetPrefixedRCFloat(this->prefix, pszSetting, this->group != NULL ? this->group->GetFloat(pszSetting, fDefault) : fDefault);
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
    return GetPrefixedRCDouble(this->prefix, pszSetting, this->group != NULL ? this->group->GetDouble(pszSetting, dDefault) : dDefault);
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
    return GetPrefixedRCBool(this->prefix, pszSetting, this->group != NULL ? this->group->GetBool(pszSetting, bDefault) : bDefault);
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
    return GetPrefixedRCMonitor(this->prefix, pszSetting, this->group != NULL ? this->group->GetMonitor(pszSetting, uDefault) : uDefault);
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
