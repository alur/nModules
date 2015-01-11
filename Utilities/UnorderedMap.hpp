/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  UnorderedMap.hpp
*  The nModules Project
*
*  An extendable version of unordered_map. It's not really complete yet, extend
*  as required.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "StringUtils.h"

#include <tchar.h>
#include <unordered_map>


/// <summary>
/// Extension of std::unordered_map.
/// </summary>
template <
  typename KeyType,
  typename ValueType,
  typename KeyHasher = std::hash<KeyType>,
  typename KeyComparator = std::equal_to<KeyType>,
  typename Allocator = std::allocator<std::pair<const KeyType, ValueType>>
>
class UnorderedMap {
protected:
  typedef std::unordered_map<KeyType, ValueType, KeyHasher, KeyComparator, Allocator> MapType;
  MapType mMap;

public:
  typedef typename MapType::const_iterator ConstIterator;
  typedef typename MapType::iterator Iterator;

public:
  /// <summary>
  /// Default constructor
  /// </summary>
  UnorderedMap()
    : mMap() {}


  /// <summary>
  /// Initializer list constructor
  /// </summary>
  UnorderedMap(std::initializer_list<typename MapType::value_type> items)
    : mMap(items) {}


  /// <summary>
  /// Returns an iterator pointing to the first element in the container.
  /// </summary>
  Iterator begin() {
    return mMap.begin();
  }


  /// <summary>
  /// Returns an iterator pointing to the past-the-end element in the container.
  /// </summary>
  Iterator end() {
    return mMap.end();
  }


  /// <summary>
  /// Emplaces the element into the container.
  /// </summary>
  /// <param name="args"></param>
  template <typename... Args>
  std::pair<Iterator, bool> Emplace(Args && ... args) {
    return mMap.emplace(std::forward<Args>(args)...);
  }


  /// <summary>
  /// Retrieves a value from the map.
  /// </summary>
  /// <param name="key">The key of the value to retrieve.</param>
  /// <param name="defaultValue">Value to return if the key doesn't exist in the map.</param>
  ValueType & Get(const KeyType & key, std::add_const<ValueType> & defaultValue) {
    Iterator value = mMap.find(key);
    if (value != mMap.end()) {
      return value->second;
    }
    return defaultValue;
  }


  /// <summary>
  /// Retrieves a value from the map.
  /// </summary>
  /// <param name="key">The key of the value to retrieve.</param>
  /// <param name="defaultValue">Value to return if the key doesn't exist in the map.</param>
  const ValueType & Get(const KeyType & key, const ValueType & defaultValue) const {
    ConstIterator value = mMap.find(key);
    if (value != mMap.end()) {
      return value->second;
    }
    return defaultValue;
  }
};


/// <summary>
/// Unordered map using LPCTSTRs for keys.
/// </summary>
template <typename Type>
using UnorderedCStringMap = UnorderedMap<LPCTSTR, Type, CaseSensitive::Hash, CaseSensitive::Equal>;


/// <summary>
/// Unordered map using case-insensitive LPCTSTRs for keys.
/// </summary>
template <typename Type>
using UnorderedCaselessCStringMap = UnorderedMap<LPCTSTR, Type, CaseInsensitive::Hash, CaseInsensitive::Equal>;
