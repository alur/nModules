//-------------------------------------------------------------------------------------------------
// /Utilities/PointerIterator.hpp
// The nModules Project
//
// Simplifies usage of iterators to pointer containers.
//-------------------------------------------------------------------------------------------------
#pragma once

template <class IteratorType, class ItemType>
class PointerIterator {
public:
  PointerIterator() {
    mValid = false;
  }

  PointerIterator(IteratorType iter) : mIter(iter) {
    mValid = true;
  }

  ItemType* operator->() {
    return *mIter;
  }

  ItemType& operator*() {
    return *(*mIter);
  }

  bool operator==(IteratorType iter) {
    return mIter == iter;
  }

  bool operator!=(IteratorType iter) {
    return mIter != iter;
  }

  bool operator==(PointerIterator iter) {
    return mIter == iter.mIter;
  }

  bool operator!=(PointerIterator iter) {
    return mIter != iter.mIter;
  }

  PointerIterator & operator++() {
    ++mIter;
    return *this;
  }

  PointerIterator operator++(int) {
    mIter++;
    return *this;
  }

  PointerIterator & operator--() {
    --mIter;
    return *this;
  }

  PointerIterator operator--(int) {
    mIter--;
    return *this;
  }

  IteratorType mIter;
  bool mValid;
};
