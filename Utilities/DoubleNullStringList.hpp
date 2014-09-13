//-------------------------------------------------------------------------------------------------
// /Utilities/DoubleNullStringList.hpp
// The nModules Project
//
// Builds a list of null-separated C strings, with a double null at the end.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Common.h"

class DoubleNullStringList {
public:
  DoubleNullStringList();
  ~DoubleNullStringList();

public:
  size_t GetStringSize() const;
  LPCWSTR GetStringList() const;
  void Push(LPCWSTR);

private:
  size_t mCchSize;
  LPWSTR mStringList;
};
