//-------------------------------------------------------------------------------------------------
// /Utilities/AlgorithmExtension.h
// The nModules Project
//
// Extends the <algorithm> header.
//-------------------------------------------------------------------------------------------------
#pragma once

namespace std {
  /// <summary>
  /// Gets a value from a map or the default value.
  /// </summary>
  template <template<class, class, class...> class C, typename K, typename V, typename... Args>
  V get(const C<K, V, Args...>& m, K const& key, const V & defval) {
    typename C<K, V, Args...>::const_iterator it = m.find(key);
    return it == m.end() ? defval : it->second;
  }
}
