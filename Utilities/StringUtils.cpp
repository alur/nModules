//--------------------------------------------------------------------------------------
// StringUtils.h
// The nModules Project
//
// Utilities for dealing with strings.
//
//--------------------------------------------------------------------------------------
#include "../nShared/LiteStep.h"
#include "StringUtils.h"


/// <summary>
/// Duplicates the first cch characters from str.
/// </summary>
/// <param name="str"></param>
/// <param name="cch"></param>
LPWSTR StringUtils::PartialDup(LPCWSTR str, size_t cch) {
    LPWSTR ret = (LPWSTR)malloc((cch+1)*sizeof(WCHAR));
    memcpy(ret, str, cch*sizeof(WCHAR));
    ret[cch] = '\0';
    return ret;
}


/// <summary>
/// Reallocates dest to contain the string in str, and overwrites dest with str.
/// </summary>
/// <param name="dest"></param>
/// <param name="str"></param>
LPSTR StringUtils::ReallocOverwrite(LPSTR dest, LPCSTR str) {
    size_t cch = strlen(str);
    LPSTR ret = (LPSTR)realloc(dest, cch+1);
    memcpy(ret, str, cch);
    ret[cch] = '\0';
    return ret;
}


/// <summary>
/// Reallocates dest to contain the string in str, and overwrites dest with str.
/// </summary>
/// <param name="dest"></param>
/// <param name="str"></param>
LPWSTR StringUtils::ReallocOverwrite(LPWSTR dest, LPCWSTR str) {
    size_t cch = wcslen(str);
    LPWSTR ret = (LPWSTR)realloc(dest, (cch+1)*sizeof(WCHAR));
    memcpy(ret, str, cch*sizeof(WCHAR));
    ret[cch] = '\0';
    return ret;
}



/// <summary>
/// 
/// </summary>
/// <param name="str"></param>
/// <param name="tokens"></param>
//std::vector<std::wstring> Explode(LPCWSTR str, LPCWSTR tokens) {

//}
