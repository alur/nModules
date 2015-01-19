//-------------------------------------------------------------------------------------------------
// /Utilities/UIDGenerator.hpp
// The nModules Project
//
// Manages a set of unique ID's, letting you generate and release them.
//-------------------------------------------------------------------------------------------------
#pragma once

#include <list>

template <class Type>
class UIDGenerator {
public:
  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="start">The ID to start at.</param>
  explicit UIDGenerator(Type start = 0) {
    mNextFreshID = start;
  }

  /// <summary>
  /// Generates a new ID. This ID won't be generated again until it has been released.
  /// </summary>
  /// <returns>The unique ID.</returns>
  Type GetNewID() {
    if (mReleasedIDs.empty()) {
      return mNextFreshID++;
    }
    Type ret = mReleasedIDs.front();
    mReleasedIDs.pop_front();
    return ret;
  }

  /// <summary>
  /// Releases an ID, allowing it to be generated again.
  /// </summary>
  /// <param name="id">The ID to release.</param>
  void ReleaseID(Type id) {
    mReleasedIDs.push_back(id);
  }

private:
  Type mNextFreshID;
  std::list<Type> mReleasedIDs;
};
