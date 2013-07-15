//--------------------------------------------------------------------------------------
// StringUtils.h
// The nModules Project
//
// Utilities for dealing with strings.
//
//--------------------------------------------------------------------------------------
#pragma once

#include "Iterator.hpp"
#include <vector>

namespace StringUtils {
    LPWSTR PartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);

    //Iterator<LPCWSTR> Explode(LPCWSTR str, LPCWSTR tokens);
    std::vector<std::wstring> Explode(LPCWSTR str, LPCWSTR tokens);
}
