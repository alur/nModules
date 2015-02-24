#pragma once

#include "../nUtilities/Windows.h"

#include <strsafe.h>


static inline void ConcatenateStrings(LPWSTR dest, size_t cchDest, LPCWSTR string1,
    LPCWSTR string2) {
  StringCchCopyEx(dest, cchDest, string1, &dest, &cchDest, 0);
  StringCchCopy(dest, cchDest, string2);
}
