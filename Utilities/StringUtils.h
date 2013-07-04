//--------------------------------------------------------------------------------------
// StringUtils.h
// The nModules Project
//
// Utilities for dealing with strings.
//
//--------------------------------------------------------------------------------------
#pragma once

namespace StringUtils {
    LPWSTR PartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);
}
