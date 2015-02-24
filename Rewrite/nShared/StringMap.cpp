#include "StringMap.hpp"

#include <strsafe.h>


StringMap::StringMap(
    std::initializer_list<std::unordered_map<std::wstring, std::wstring>::value_type> values)
  : mMap(values) {}


bool StringMap::Get(LPCWSTR key, LPWSTR buffer, size_t cchBuffer) const {
  auto iter = mMap.find(key);
  if (iter == mMap.end()) {
    return false;
  }
  StringCchCopy(buffer, cchBuffer, iter->second.c_str());
  return true;
}
