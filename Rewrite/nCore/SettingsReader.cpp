#include "Api.h"
#include "SettingsReader.hpp"

#include "../nShared/String.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <assert.h>
#include <functional>
#include <strsafe.h>


EXPORT_CDECL(ISettingsReader*) CreateSettingsReader(LPCWSTR prefix, const IStringMap *defaults) {
  return SettingsReader::Create(prefix, defaults);
}


static bool GetPrefixedRCString(LPCWSTR prefix, LPCWSTR key, LPWSTR buffer, size_t cchBuffer,
    LPCWSTR defaultVal) {
  // This is bad, since the first thing GetRCString does is to set *buffer = '\0'
  assert(buffer != defaultVal);

  wchar_t prefixedKey[MAX_PREFIX];
  ConcatenateStrings(prefixedKey, _countof(prefixedKey), prefix, key);
  return GetRCString(prefixedKey, buffer, defaultVal, (UINT)cchBuffer) != FALSE;
}


ISettingsReader *SettingsReader::Create(LPCWSTR prefix, const IStringMap *defaults) {
  SettingsReader *reader = new SettingsReader(defaults);

  reader->mPrefixes.emplace_back();
  StringCchCopy(reader->mPrefixes.back(), MAX_PREFIX, prefix);

  wchar_t group[MAX_PREFIX];
  while (GetPrefixedRCString(reader->mPrefixes.back(), L"Group", group, MAX_PREFIX, L"")) {
    reader->mPrefixes.emplace_back();
    StringCchCopy(reader->mPrefixes.back(), MAX_PREFIX, group);
    // TODO(Erik): Check for loops.
  }

  return reader;
}


SettingsReader::SettingsReader(const IStringMap *defaults) : mDefaults(defaults) {
  *mDefaultsPrefix = L'\0';
}


ISettingsReader *SettingsReader::CreateChild(LPCWSTR suffix) const {
  SettingsReader *reader = new SettingsReader(mDefaults);
  for (LPCWSTR prefix : mPrefixes) {
    reader->mPrefixes.emplace_back();
    ConcatenateStrings(reader->mPrefixes.back(), MAX_PREFIX, prefix, suffix);

    wchar_t group[MAX_PREFIX];
    while (GetPrefixedRCString(reader->mPrefixes.back(), L"Group", group, MAX_PREFIX, L"")) {
      reader->mPrefixes.emplace_back();
      StringCchCopy(reader->mPrefixes.back(), MAX_PREFIX, group);
      // TODO(Erik): Check for loops.
    }
  }
  if (mDefaults) {
    ConcatenateStrings(reader->mDefaultsPrefix, MAX_PREFIX, mDefaultsPrefix, suffix);
  }
  return reader;
}


void SettingsReader::Discard() {
  delete this;
}


void SettingsReader::EnumCommandLines(LPCWSTR key,
    void (APICALL *callback)(LPCWSTR line, LPARAM lParam), LPARAM lParam) const {
  wchar_t prefixedKey[MAX_PREFIX];
  prefixedKey[0] = '*';
  for (LPCWSTR prefix : mPrefixes) {
    ConcatenateStrings(prefixedKey + 1, _countof(prefixedKey) - 1, prefix, key);
    EnumRCLines(prefixedKey, callback, lParam);
  }
}


void SettingsReader::EnumLines(LPCWSTR key, void (APICALL *callback)(LPCWSTR line, LPARAM lParam),
    LPARAM lParam) const {
  wchar_t prefixedKey[MAX_PREFIX];
  for (LPCWSTR prefix : mPrefixes) {
    ConcatenateStrings(prefixedKey, _countof(prefixedKey), prefix, key);
    EnumRCLines(prefixedKey, callback, lParam);
  }
}


bool SettingsReader::GetBool(LPCWSTR key, bool defaultValue) const {
  bool val;
  return GetBool(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetBool(LPCWSTR key, bool *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    *value = _wcsicmp(buffer, L"off") != 0 && _wcsicmp(buffer, L"false") != 0 &&
      _wcsicmp(buffer, L"no") != 0;
    return true;
  }

  return false;
}


double SettingsReader::GetDouble(LPCWSTR key, double defaultValue) const {
  double val;
  return GetDouble(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetDouble(LPCWSTR key, double *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    *value = wcstod(buffer, nullptr);
    return true;
  }

  return false;
}


float SettingsReader::GetFloat(LPCWSTR key, float defaultValue) const {
  float val;
  return GetFloat(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetFloat(LPCWSTR key, float *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    *value = wcstof(buffer, nullptr);
    return true;
  }

  return false;
}


int SettingsReader::GetInt(LPCWSTR key, int defaultValue) const {
  int val;
  return GetInt(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetInt(LPCWSTR key, int *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    *value = wcstol(buffer, nullptr, 0);
    return true;
  }

  return false;
}


__int64 SettingsReader::GetInt64(LPCWSTR key, __int64 defaultValue) const {
  __int64 val;
  return GetInt64(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetInt64(LPCWSTR key, __int64 *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    *value = _wcstoi64(buffer, nullptr, 0);
    return true;
  }

  return false;
}


bool SettingsReader::GetFromDefaults(LPCWSTR key, LPWSTR value, size_t cchValue) const {
  if (mDefaults) {
    wchar_t prefixedKey[MAX_PREFIX];
    ConcatenateStrings(prefixedKey, MAX_PREFIX, mDefaultsPrefix, key);
    if (mDefaults->Get(prefixedKey, value, cchValue)) {
      return true;
    }
  }
  return false;
}


bool SettingsReader::GetString(LPCWSTR key, LPWSTR value, size_t cchValue,
    LPWSTR defaultValue) const {
  for (LPCWSTR prefix : mPrefixes) {
    if (GetPrefixedRCString(prefix, key, value, cchValue, defaultValue)) {
      return true;
    }
  }
  if (GetFromDefaults(key, value, cchValue)) {
    return true;
  }
  return false;
}


NLENGTH SettingsReader::GetLength(LPCWSTR key, const NLENGTH &defaultValue) const {
  NLENGTH val;
  return GetLength(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetLength(LPCWSTR key, NLENGTH *value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    return ParseLength(buffer, value);
  }

  return false;
}


UINT SettingsReader::GetMonitor(LPCWSTR key, const UINT defaultValue) const {
  UINT val;
  return GetMonitor(key, &val) ? val : defaultValue;
}


bool SettingsReader::GetMonitor(LPCWSTR key, const LPUINT value) const {
  wchar_t buffer[MAX_LINE_LENGTH];

  if (GetString(key, buffer, MAX_LINE_LENGTH, L"")) {
    return ParseMonitor(buffer, value);
  }

  return false;
}


NRECT SettingsReader::GetXYWHRect(LPCWSTR, const NRECT&) const {
  assert(false); // Not implemented
  return NRECT();
}


bool SettingsReader::GetXYWHRect(LPCWSTR, NRECT*) const {
  assert(false); // Not implemented
  return false;
}


NRECT SettingsReader::GetLTRBRect(LPCWSTR key, const NRECT &defaultValue) const {
  NRECT rect;
  wchar_t buffer[MAX_PREFIX];

  ConcatenateStrings(buffer, MAX_PREFIX, key, L"Left");
  rect.left = GetLength(buffer, defaultValue.left);

  ConcatenateStrings(buffer, MAX_PREFIX, key, L"Top");
  rect.top = GetLength(buffer, defaultValue.top);

  ConcatenateStrings(buffer, MAX_PREFIX, key, L"Right");
  rect.right = GetLength(buffer, defaultValue.right);

  ConcatenateStrings(buffer, MAX_PREFIX, key, L"Bottom");
  rect.bottom = GetLength(buffer, defaultValue.bottom);

  return rect;
}


bool SettingsReader::GetLTRBRect(LPCWSTR, NRECT*) const {
  assert(false); // Not implemented
  return false;
}
