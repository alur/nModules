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


/// <summary>
/// Hash and comparison functions for C-style strings.
/// </summary>
struct CStringComparator
{
    size_t operator()(LPCTSTR str) const
    {
#if defined(_WIN64)
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        const size_t _FNV_offset_basis = 14695981039346656037ULL;
        const size_t _FNV_prime = 1099511628211ULL;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
        const size_t _FNV_offset_basis = 2166136261U;
        const size_t _FNV_prime = 16777619U;
#endif
        size_t value = _FNV_offset_basis;
        for (LPCTSTR chr = str; *chr != 0; ++chr)
        {
            value ^= (size_t)*chr;
            value *= _FNV_prime;
        }

#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        value ^= value >> 32;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif

        return value;
    }

    bool operator()(LPCTSTR a, LPCTSTR b) const
    {
        return _tcscmp(a, b) == 0;
    }
};


/// <summary>
/// Hash and comparison functions for case-insensitive C-style strings.
/// </summary>
struct CStringComparatorNoCase
{
    size_t operator()(LPCTSTR str) const
    {
#if defined(_WIN64)
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        const size_t _FNV_offset_basis = 14695981039346656037ULL;
        const size_t _FNV_prime = 1099511628211ULL;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
        const size_t _FNV_offset_basis = 2166136261U;
        const size_t _FNV_prime = 16777619U;
#endif
        size_t value = _FNV_offset_basis;
        for (LPCTSTR chr = str; *chr != 0; ++chr)
        {
            value ^= (size_t)_totlower(*chr);
            value *= _FNV_prime;
        }

#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        value ^= value >> 32;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif

        return value;
    }

    bool operator()(LPCTSTR a, LPCTSTR b) const
    {
        return _tcsicmp(a, b) == 0;
    }
};


inline size_t _Hash_seq(const unsigned char *_First, size_t _Count)
{	// FNV-1a hash function for bytes in [_First, _First+_Count)
#if defined(_M_X64) || defined(_LP64) || defined(__x86_64) || defined(_WIN64)
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    const size_t _FNV_offset_basis = 14695981039346656037ULL;
    const size_t _FNV_prime = 1099511628211ULL;

#else /* defined(_M_X64), etc. */
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    const size_t _FNV_offset_basis = 2166136261U;
    const size_t _FNV_prime = 16777619U;
#endif /* defined(_M_X64), etc. */

    size_t _Val = _FNV_offset_basis;
    for (size_t _Next = 0; _Next < _Count; ++_Next)
    {	// fold in another byte
        _Val ^= (size_t)_First[_Next];
        _Val *= _FNV_prime;
    }

    return (_Val);
}


namespace StringUtils
{
    LPSTR PartialDup(LPCSTR str, size_t cch);
    LPWSTR PartialDup(LPCWSTR str, size_t cch);
    LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
    LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);
}
