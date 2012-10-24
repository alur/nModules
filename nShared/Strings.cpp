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
