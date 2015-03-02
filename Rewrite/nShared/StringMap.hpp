#pragma once

#include "String.h"

#include "../nCoreApi/IStringMap.hpp"

#include <string>

class StringMap : public IStringMap {
private:
  using MapType = StringKeyedMaps<std::wstring, std::wstring>::UnorderedMap;

public:
  explicit StringMap(std::initializer_list<MapType::value_type>);

  // IStringMap
public:
  bool APICALL Get(LPCWSTR key, LPWSTR buffer, size_t cchBuffer) const override;

private:
  MapType mMap;
};
