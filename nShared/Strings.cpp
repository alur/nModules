/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Strings.cpp
 *  The nModules Project
 *
 *  Utility functions for dealing with strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "Strings.h"


LPWSTR Strings::wcsPartialDup(LPCWSTR str, size_t cch) {
    LPWSTR ret = (LPWSTR)malloc((cch+1)*sizeof(WCHAR));
    memcpy(ret, str, cch*sizeof(WCHAR));
    ret[cch] = '\0';
    return ret;
}


LPSTR Strings::ReallocOverwrite(LPSTR dest, LPCSTR str) {
    size_t cch = strlen(str);
    LPSTR ret = (LPSTR)realloc(dest, cch+1);
    memcpy(ret, str, cch);
    ret[cch] = '\0';
    return ret;
}


LPWSTR Strings::ReallocOverwriteW(LPWSTR dest, LPCWSTR str) {
    size_t cch = wcslen(str);
    LPWSTR ret = (LPWSTR)realloc(dest, (cch+1)*sizeof(WCHAR));
    memcpy(ret, str, cch*sizeof(WCHAR));
    ret[cch] = '\0';
    return ret;
}
