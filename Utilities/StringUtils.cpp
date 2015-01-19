//-------------------------------------------------------------------------------------------------
// /Utilities/StringUtils.cpp
// The nModules Project
//
// Utilities for dealing with strings.
//-------------------------------------------------------------------------------------------------
#include "Common.h"
#include "StringUtils.h"


/// <summary>
/// Duplicates the first cch characters from str.
/// </summary>
/// <param name="str">The string to duplicate.</param>
/// <param name="cch">The number of characters to duplicate.</param>
/// <returns>The new string, allocated using malloc.</returns>
LPSTR StringUtils::PartialDup(LPCSTR str, size_t cch) {
  LPSTR ret = (LPSTR)malloc((cch + 1));
  memcpy(ret, str, cch);
  ret[cch] = '\0';
  return ret;
}


/// <summary>
/// Duplicates the first cch characters from str.
/// </summary>
/// <param name="str">The string to duplicate.</param>
/// <param name="cch">The number of characters to duplicate.</param>
/// <returns>The new string, allocated using malloc.</returns>
LPWSTR StringUtils::PartialDup(LPCWSTR str, size_t cch) {
  LPWSTR ret = (LPWSTR)malloc((cch + 1)*sizeof(WCHAR));
  memcpy(ret, str, cch*sizeof(WCHAR));
  ret[cch] = L'\0';
  return ret;
}


/// <summary>
/// Reallocates dest to contain the string in str, and overwrites dest with str.
/// When this function returns, dest will have been freed.
/// </summary>
/// <param name="dest">The string to reallocate.</param>
/// <param name="str">The string to replace dest with.</param>
/// <returns>A pointer to the new string.</returns>
LPSTR StringUtils::ReallocOverwrite(LPSTR dest, LPCSTR str) {
  size_t cch = strlen(str);
  LPSTR ret = (LPSTR)realloc(dest, cch + 1);
  memcpy(ret, str, cch);
  ret[cch] = '\0';
  return ret;
}


/// <summary>
/// Reallocates dest to contain the string in str, and overwrites dest with str.
/// When this function returns, dest will have been freed.
/// </summary>
/// <param name="dest">The string to reallocate.</param>
/// <param name="str">The string to replace dest with.</param>
/// <returns>A pointer to the new string.</returns>
LPWSTR StringUtils::ReallocOverwrite(LPWSTR dest, LPCWSTR str) {
  size_t cch = wcslen(str);
  LPWSTR ret = (LPWSTR)realloc(dest, (cch + 1)*sizeof(WCHAR));
  memcpy(ret, str, cch*sizeof(WCHAR));
  ret[cch] = L'\0';
  return ret;
}
