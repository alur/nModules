//-------------------------------------------------------------------------------------------------
// /Utilities/DoubleNullStringList.cpp
// The nModules Project
//
// Builds a list of null-separated C strings, with a double null at the end.
//-------------------------------------------------------------------------------------------------
#include "DoubleNullStringList.hpp"

#include <stdlib.h>


DoubleNullStringList::DoubleNullStringList()
  : mCchSize(2)
  , mStringList((LPWSTR)calloc(2, sizeof(wchar_t))) {}


DoubleNullStringList::~DoubleNullStringList() {
  free(mStringList);
}


size_t DoubleNullStringList::GetStringSize() const {
  return mCchSize * sizeof(wchar_t);
}


LPCWSTR DoubleNullStringList::GetStringList() const {
  return mStringList;
}


void DoubleNullStringList::Push(LPCWSTR string) {
  size_t cchStrLen = wcslen(string) + 1;
  mStringList = LPWSTR(realloc(mStringList, sizeof(wchar_t)* (mCchSize + cchStrLen)));
  memcpy(&mStringList[mCchSize - 2], string, (cchStrLen)* sizeof(wchar_t));
  mCchSize += cchStrLen;
  mStringList[mCchSize - 2] = L'\0';
  mStringList[mCchSize - 1] = L'\0';
}
