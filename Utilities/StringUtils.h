//-------------------------------------------------------------------------------------------------
// /Utilities/StringUtils.h
// The nModules Project
//
// Utilities for dealing with strings.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Common.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace StringUtils {
  LPSTR PartialDup(LPCSTR str, size_t cch);
  LPWSTR PartialDup(LPCWSTR str, size_t cch);
  LPSTR ReallocOverwrite(LPSTR dest, LPCSTR str);
  LPWSTR ReallocOverwrite(LPWSTR dest, LPCWSTR str);
}


// <summary>
// Hashing function for null-terminated strings.
// </summary>
template <
  typename CharPtrType,
  typename CharTransformer
>
struct StringHasher {
  size_t operator()(CharPtrType str) const {
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
    for (CharPtrType chr = str; *chr != 0; ++chr) {
      value ^= (size_t)CharTransformer()(*chr);
      value *= _FNV_prime;
    }

#if defined(_WIN64)
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    value ^= value >> 32;
#else
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif
    return value;
  }
};


// <summary>
// Case sensitive functions for StringKeyedContainers.
// </summary>
struct CaseSensitive {
  struct Hash {
    size_t operator()(LPCWSTR str) const {
      struct CharTransformer {
        inline size_t operator()(wchar_t chr) {
          return (size_t)chr;
        }
      };
      return StringHasher<LPCWSTR, CharTransformer>()(str);
    }

    size_t operator()(LPCSTR str) const {
      struct CharTransformer {
        inline size_t operator()(char chr) {
          return (size_t)chr;
        }
      };
      return StringHasher<LPCSTR, CharTransformer>()(str);
    }

    size_t operator()(const std::wstring & str) const {
      return operator()(str.c_str());
    }

    size_t operator()(const std::string & str) const {
      return operator()(str.c_str());
    }
  };

  struct Compare {
    bool operator()(LPCWSTR a, LPCWSTR b) const {
      return wcscmp(a, b) < 0;
    }

    bool operator()(LPCSTR a, LPCSTR b) const {
      return strcmp(a, b) < 0;
    }

    bool operator()(const std::wstring & a, const std::wstring & b) const {
      return wcscmp(a.c_str(), b.c_str()) < 0;
    }

    bool operator()(const std::string & a, const std::string & b) const {
      return strcmp(a.c_str(), b.c_str()) < 0;
    }
  };

  struct Equal {
    bool operator()(LPCWSTR a, LPCWSTR b) const {
      return wcscmp(a, b) == 0;
    }

    bool operator()(LPCSTR a, LPCSTR b) const {
      return strcmp(a, b) == 0;
    }

    bool operator()(const std::wstring & a, const std::wstring & b) const {
      return wcscmp(a.c_str(), b.c_str()) == 0;
    }

    bool operator()(const std::string & a, const std::string & b) const {
      return strcmp(a.c_str(), b.c_str()) == 0;
    }
  };
};


// <summary>
// Case insensitive functions for StringKeyedContainers.
// </summary>
struct CaseInsensitive {
  struct Hash {
    size_t operator()(LPCWSTR str) const {
      struct CharTransformer {
        inline size_t operator()(wchar_t chr) {
          return (size_t)towlower(chr);
        }
      };
      return StringHasher<LPCWSTR, CharTransformer>()(str);
    }

    size_t operator()(LPCSTR str) const {
      struct CharTransformer {
        inline size_t operator()(char chr) {
          return (size_t)tolower(chr);
        }
      };
      return StringHasher<LPCSTR, CharTransformer>()(str);
    }

    size_t operator()(const std::wstring & str) const {
      return operator()(str.c_str());
    }

    size_t operator()(const std::string & str) const {
      return operator()(str.c_str());
    }
  };

  struct Compare {
    bool operator()(LPCSTR a, LPCSTR b) const {
      return _stricmp(a, b) < 0;
    }

    bool operator()(LPCWSTR a, LPCWSTR b) const {
      return _wcsicmp(a, b) < 0;
    }

    bool operator()(const std::string & a, const std::string & b) const {
      return _stricmp(a.c_str(), b.c_str()) < 0;
    }

    bool operator()(const std::wstring & a, const std::wstring & b) const {
      return _wcsicmp(a.c_str(), b.c_str()) < 0;
    }
  };

  struct Equal {
    bool operator()(LPCSTR a, LPCSTR b) const {
      return _stricmp(a, b) == 0;
    }

    bool operator()(LPCWSTR a, LPCWSTR b) const {
      return _wcsicmp(a, b) == 0;
    }

    bool operator()(const std::string & a, const std::string & b) const {
      return _stricmp(a.c_str(), b.c_str()) == 0;
    }

    bool operator()(const std::wstring & a, const std::wstring & b) const {
      return _wcsicmp(a.c_str(), b.c_str()) == 0;
    }
  };
};


// <summary>
// Standard maps using strings as keys.
// </summary>
template <
  typename KeyType,
  typename Type,
  typename KeyOperators = CaseInsensitive,
  typename Allocator = std::allocator < std::pair<std::add_const<KeyType>, Type> >
>
struct StringKeyedMaps {
  using Map = std::map < KeyType, Type, typename KeyOperators::Compare, Allocator > ;
  using ConstMap = const Map;
  using MultiMap = std::multimap < KeyType, Type, typename KeyOperators::Compare, Allocator > ;
  using ConstMultiMap = const MultiMap;
  using UnorderedMap = std::unordered_map < KeyType, Type, typename KeyOperators::Hash, typename KeyOperators::Equal, Allocator > ;
  using ConstUnorderedMap = const UnorderedMap;
  using UnorderedMultiMap = std::unordered_multimap < KeyType, Type, typename KeyOperators::Hash, typename KeyOperators::Equal, Allocator > ;
  using ConstUnorderedMultiMap = const UnorderedMultiMap;
};


// <summary>
// Standard sets using strings as keys.
// </summary>
template <
  typename Type,
  typename Operators = CaseInsensitive,
  typename Allocator = std::allocator < std::pair<std::add_const<Type>, Type> >
>
struct StringKeyedSets {
  using Set = std::set < Type, typename Operators::Compare, Allocator > ;
  using ConstSet = const Set;
  using MultiSet = std::multiset < Type, typename Operators::Compare, Allocator > ;
  using ConstMultiSet = const MultiSet;
  using UnorderedSet = std::unordered_set < Type, typename Operators::Hash, typename Operators::Equal, Allocator > ;
  using ConstUnorderedSet = const UnorderedSet;
  using UnorderedMultiSet = std::unordered_multiset < Type, typename Operators::Hash, typename Operators::Equal, Allocator > ;
  using ConstUnorderedMultiSet = const UnorderedMultiSet;
};
