#include "Api.h"

#include <wchar.h>


EXPORT_CDECL(bool) ParseLength(LPCWSTR string, NLENGTH *out) {
  if (string == nullptr || out == nullptr || *string == L'\0') {
    return false;
  }

  float pixels = 0;
  float percent = 0;
  float dips = 0;

  float *defaultType = &dips;

  // Look at the string as a collection of tokens, delimited by + and -
  while (*string) {
    float number = wcstof(string, const_cast<LPWSTR*>(&string));
    if (*string == L'%') {
      percent += number;
      ++string;
    } else if (string[0] == L'd' && string[1] == L'i' && string[2] == L'p') {
      dips += number;
      string += 3;
    } else if (string[0] == L'p' && string[1] == L'x') {
      pixels += number;
      string += 2;
    } else {
      *defaultType += number;
    }

    // The next token has to be a +, -, or end of string.
    if (*string != L'+' && *string != L'-' && *string != L'\0') {
      return false;
    }
  }

  *out = NLENGTH(pixels, percent/100.0f, dips);

  return true;
}


EXPORT_CDECL(bool) ParseMonitor(LPCWSTR string, LPUINT out) {
  static const struct {
    LPCTSTR name;
    UINT value;
  } monitorMap[] =  {
    { L"primary",       0 },
    { L"secondary",     1 },
    { L"tertiary",      2 },
    { L"quaternary",    3 },
    { L"quinary",       4 },
    { L"senary",        5 },
    { L"septenary",     6 },
    { L"octonary",      7 },
    { L"nonary",        8 },
    { L"denary",        9 },
    { L"duodenary",    11 },
    { L"all", MONITOR_ALL }
  };

  if (string == nullptr || out == nullptr) {
    return false;
  }

  // First check if the string is a named value
  for (auto item : monitorMap) {
    if (_wcsicmp(item.name, string) == 0) {
      *out = item.value;
      return true;
    }
  }

  // Then try to parse the string as an integer
  LPTSTR endPtr;
  UINT monitor = wcstoul(string, &endPtr, 0);
  if (*string != L'\0' && *endPtr == L'\0') {
    *out = monitor;
    return true;
  }

  return false;
}
