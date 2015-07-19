#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <functional>


EXPORT_CDECL(void) EnumTokens(LPCWSTR line, void(*callback)(LPCWSTR, LPARAM), LPARAM data) {
  wchar_t token[MAX_LINE_LENGTH];
  while (GetToken(line, token, &line, false)) {
    callback(token, data);
  }
}


EXPORT_CDECL(void) EnumRCLines(LPCWSTR key, void(*callback)(LPCWSTR, LPARAM), LPARAM data) {
  wchar_t line[MAX_LINE_LENGTH];

  // Lines are of the form Key Value
  LPCWSTR valuePart = line + wcslen(key) + 1;
  LPVOID f = LCOpen(nullptr);
  while (LCReadNextConfig(f, key, line, _countof(line))) {
    callback(valuePart, data);
  }
  LCClose(f);
}


EXPORT_CDECL(void) EnumRCLineTokens(LPCWSTR key, void(*callback)(LPCWSTR, LPARAM), LPARAM data) {
  wchar_t line[MAX_LINE_LENGTH];

  // Lines are of the form Key Value
  LPCWSTR valuePart = line + wcslen(key) + 1;
  LPVOID f = LCOpen(nullptr);
  while (LCReadNextConfig(f, key, line, _countof(line))) {
    EnumTokens(valuePart, callback, data);
  }
  LCClose(f);
}
