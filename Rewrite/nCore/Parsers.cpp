#include "Parsers.hpp"

#include <wchar.h>


bool ParseLength(LPCWSTR string, NLENGTH *out) {
  if (string == nullptr || out == nullptr || *string == L'\0') {
    return false;
  }

  float pixels = 0;
  float percent = 0;
  float dips = 0;

  // Look at the string as a collection of tokens, delimited by + and -
  while (*string) {
    float number = wcstof(string, const_cast<LPWSTR*>(&string));
    if (*string == L'%') {
      percent += number;
      ++string;
    } else if (string[0] == L'd' && string[1] == L'i' && string[2] == L'p') {
      dips += number;
      string += 3;
    } else {
      pixels += number;
    }

    // The next token has to be a +, -, or end of string.
    if (*string != L'+' && *string != L'-' && *string != L'\0') {
      return false;
    }
  }

  *out = NLENGTH(pixels, percent, dips);

  return true;
}
