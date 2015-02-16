#pragma once

#include "../nCoreApi/IStringMap.hpp"

#include <string>
#include <unordered_map>

class StringMap : public IStringMap {
public:
  explicit StringMap(
    std::initializer_list<std::unordered_map<std::wstring, std::wstring>::value_type>);

  // IStringMap
public:
  bool APICALL Get(LPCWSTR key, LPWSTR buffer, size_t cchBuffer) const override;

private:
  std::unordered_map<std::wstring, std::wstring> mMap;
};
