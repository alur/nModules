/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InputParsing.cpp                                                July, 2012
 *  The nModules Project
 *
 *  Provides calls to the inputparsing functions of the core.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Core.h"

// Pointers to the functions in the core.
bool (__cdecl * _pParseCoordinate)(LPCSTR, int *, bool, bool);
bool (__cdecl * _pParseLength)(LPCSTR, int *, bool, bool);
bool (__cdecl * _pParseMonitor)(LPCSTR, UINT *);
bool (__cdecl * _pParseBool)(LPCSTR);
int (__cdecl * _pGetPrefixedRCInt)(LPCSTR, LPCSTR, int);
float (__cdecl * _pGetPrefixedRCFloat)(LPCSTR, LPCSTR, float);
double (__cdecl * _pGetPrefixedRCDouble)(LPCSTR, LPCSTR, double);
ARGB (__cdecl * _pGetPrefixedRCColor)(LPCSTR, LPCSTR, int);
bool (__cdecl * _pGetPrefixedRCBool)(LPCSTR, LPCSTR, bool);
bool (__cdecl * _pGetPrefixedRCString)(LPCSTR, LPCSTR, LPSTR, LPCSTR, UINT);
bool (__cdecl * _pGetPrefixedRCWString)(LPCSTR, LPCSTR, LPWSTR, LPCSTR, UINT);
UINT (__cdecl * _pGetPrefixedRCMonitor)(LPCSTR, LPCSTR, UINT);


/// <summary>
/// Initalizes the core communications.
/// </summary>
/// <returns>True if the core is succefully initalized.</returns>
HRESULT nCore::InputParsing::Init(HMODULE hCoreInstance) {
    INIT_FUNC(_pParseCoordinate,bool (__cdecl *)(LPCSTR, int *, bool, bool),"ParseCoordinateEx")
    INIT_FUNC(_pParseLength,bool (__cdecl *)(LPCSTR, int *, bool, bool),"ParseLength")
    INIT_FUNC(_pParseMonitor,bool (__cdecl *)(LPCSTR, UINT *),"ParseMonitor")
    INIT_FUNC(_pParseBool,bool (__cdecl *)(LPCSTR),"ParseBool")

    INIT_FUNC(_pGetPrefixedRCInt,int (__cdecl *)(LPCSTR, LPCSTR, int),"GetPrefixedRCInt")
    INIT_FUNC(_pGetPrefixedRCFloat,float (__cdecl *)(LPCSTR, LPCSTR, float),"GetPrefixedRCFloat")
    INIT_FUNC(_pGetPrefixedRCDouble,double (__cdecl *)(LPCSTR, LPCSTR, double),"GetPrefixedRCDouble")
    INIT_FUNC(_pGetPrefixedRCColor,ARGB (__cdecl *)(LPCSTR, LPCSTR, int),"GetPrefixedRCColor")
    INIT_FUNC(_pGetPrefixedRCBool,bool (__cdecl *)(LPCSTR, LPCSTR, bool),"GetPrefixedRCBool")
    INIT_FUNC(_pGetPrefixedRCString,bool (__cdecl *)(LPCSTR, LPCSTR, LPSTR, LPCSTR, UINT),"GetPrefixedRCString")
    INIT_FUNC(_pGetPrefixedRCWString,bool (__cdecl *)(LPCSTR, LPCSTR, LPWSTR, LPCSTR, UINT),"GetPrefixedRCWString")
    INIT_FUNC(_pGetPrefixedRCMonitor,UINT (__cdecl *)(LPCSTR, LPCSTR, UINT),"GetPrefixedRCMonitor")

    return S_OK;
}


/// <summary>
/// Parses a user inputed coordinate.
/// </summary>
/// <param name="szCoordinate">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid coordinate.</param>
/// <param name="canBeRelative">Ttrue if the coordinate can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the coordinate can be negative.</param>
/// <returns>True if szCoordinate is a valid coordinate.</return>
bool nCore::InputParsing::ParseCoordinate(LPCSTR szCoordinate, int * target, bool canBeRelative, bool canBeNegative) {
    return _pParseCoordinate(szCoordinate, target, canBeRelative, canBeNegative);
}


/// <summary>
/// Parses a user inputed length.
/// </summary>
/// <param name="szLength">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid length.</param>
/// <param name="canBeRelative">Ttrue if the length can be relative to a monitor.</param>
/// <param name="canBeNegative">True if the length can be negative.</param>
/// <returns>True if szLength is a valid length.</return>
bool nCore::InputParsing::ParseLength(LPCSTR szLength, int * target, bool canBeRelative, bool canBeNegative) {
    return _pParseLength(szLength, target, canBeRelative, canBeNegative);
}


/// <summary>
/// Parses a user inputed monitor.
/// </summary>
/// <param name="szLength">The string to parse.</param>
/// <param name="target">Pointer to an integer which will be set to the valid monitor.</param>
/// <returns>True if pszMonitor is a valid length.</return>
bool nCore::InputParsing::ParseMonitor(LPCSTR pszMonitor, UINT * target) {
    return _pParseMonitor(pszMonitor, target);
}


bool nCore::InputParsing::ParseBool(LPCSTR pszBool) {
    return _pParseBool(pszBool);
}


int nCore::InputParsing::GetPrefixedRCInt(LPCSTR szPrefix, LPCSTR szOption, int nDefault) {
    return _pGetPrefixedRCInt(szPrefix, szOption, nDefault);
}


ARGB nCore::InputParsing::GetPrefixedRCColor(LPCSTR szPrefix, LPCSTR szOption, int nDefault) {
    return _pGetPrefixedRCColor(szPrefix, szOption, nDefault);
}


bool nCore::InputParsing::GetPrefixedRCBool(LPCSTR szPrefix, LPCSTR szOption, bool bDefault) {
    return _pGetPrefixedRCBool(szPrefix, szOption, bDefault);
}


bool nCore::InputParsing::GetPrefixedRCString(LPCSTR szPrefix, LPCSTR szOption, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer) {
    return _pGetPrefixedRCString(szPrefix, szOption, pszBuffer, pszDefault, cbBuffer);
}


bool nCore::InputParsing::GetPrefixedRCWString(LPCSTR szPrefix, LPCSTR szOption, LPWSTR pszwBuffer, LPCSTR pszDefault, UINT cbBuffer) {
    return _pGetPrefixedRCWString(szPrefix, szOption, pszwBuffer, pszDefault, cbBuffer);
}


float nCore::InputParsing::GetPrefixedRCFloat(LPCSTR szPrefix, LPCSTR szOption, float fDefault) {
    return _pGetPrefixedRCFloat(szPrefix, szOption, fDefault);
}


double nCore::InputParsing::GetPrefixedRCDouble(LPCSTR szPrefix, LPCSTR szOption, double dDefault) {
    return _pGetPrefixedRCDouble(szPrefix, szOption, dDefault);
}


UINT nCore::InputParsing::GetPrefixedRCMonitor(LPCSTR szPrefix, LPCSTR szOption, UINT uDefault) {
    return _pGetPrefixedRCMonitor(szPrefix, szOption, uDefault);
}
