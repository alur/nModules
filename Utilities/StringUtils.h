/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  StringUtils.h
 *  The nModules Project
 *
 *  Utilities for dealing with strings.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <vector>
#include <algorithm>


/// <summary>
/// Case insensitive hash and equality ops
/// </summary>
struct noCaseComparator
{
    size_t operator()(const std::tstring & x) const
    {
        std::tstring copy(x);
        std::transform(x.begin(), x.end(), copy.begin(), _totlower);
        return std::hash<std::tstring>()(copy);
    }

    bool operator()(const std::tstring & a, const std::tstring & b) const
    {
        return _tcsicmp(a.c_str(), b.c_str()) == 0;
    }
};


namespace StringUtils
{
    LPSTR PartialDup(LPCSTR str, size_t cch);
    LPWSTR PartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);
}
