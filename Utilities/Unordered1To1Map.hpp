//-------------------------------------------------------------------------------------------------
// /Utilities/Unordered1To1Map.hpp
// The nModules Project
//
// A hash-based 1-to-1 map.
//-------------------------------------------------------------------------------------------------
#pragma once

#include <assert.h>
#include <forward_list>
#include <initializer_list>
#include <unordered_set>

template<
  typename TypeA,
  typename TypeB,
  typename AHash = std::hash<TypeA>,
  typename BHash = std::hash<TypeB>,
  typename AEqual = std::equal_to<TypeA>,
  typename BEqual = std::equal_to<TypeB>
>
class Unordered1To1Map {
public:
  typedef typename std::pair<TypeA, TypeB> ValueType;
  typedef typename std::forward_list<ValueType>::iterator iterator;

private:
  struct acmp {
    bool operator()(const TypeA * x, const TypeA * y) const {
      return AEqual()(*x, *y);
    }

    size_t operator()(const TypeA * x) const {
      return AHash()(*x);
    }
  };

  struct bcmp {
    bool operator()(const TypeB * x, const TypeB * y) const {
      return BEqual()(*x, *y);
    }

    size_t operator()(const TypeB * x) const {
      return BHash()(*x);
    }
  };

private:
  std::forward_list<ValueType> mData;
  std::unordered_set<TypeA *, acmp, acmp> mSetA;      // A -> Data
  std::unordered_set<TypeB *, bcmp, bcmp> mSetB;      // B -> Data

public:
  Unordered1To1Map() {}

  explicit Unordered1To1Map(const std::initializer_list<ValueType> & init)
    : mData(std::forward<const std::initializer_list<ValueType> &>(init)) {
    for (ValueType & value : mData) {
      mSetA.insert(&value.first);
      mSetB.insert(&value.second);
    }
  }

public:
  /*template<typename ... Args>
  ValueType & Insert(Args && ... args)
  {
  mData.emplace_front(std::forward<Args>(args)...);

  ValueType & value = mData.begin();
  mSetA.insert(&value.first);
  mSetB.insert(&value.second);

  return value;
  }*/

public:
  const TypeB GetByA(TypeA a, TypeB default) const {
    auto iter = mSetA.find(&a);
    if (iter != mSetA.end()) {
      return ((ValueType*)((byte*)*iter - offsetof(ValueType, first)))->second;
    }

    return default;
  }

  const TypeA GetByB(TypeB b, TypeA default) const {
    auto iter = mSetB.find(&b);
    if (iter != mSetB.end()) {
      return ((ValueType*)((byte*)*iter - offsetof(ValueType, second)))->first;
    }

    return default;
  }
};
