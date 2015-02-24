#pragma once

#include <list>

/// <summary>
/// Manages a set of unique ID's, letting you generate and release them.
/// </summary>
template <class Type> class UIDGenerator {
public:
  /// <param name="start">The ID to start at.</param>
  explicit UIDGenerator(Type start = 0) {
    mNextFreshID = start;
  }

  Type GetNewId() {
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
  void ReleaseId(Type id) {
    mReleasedIDs.push_back(id);
  }

private:
  Type mNextFreshID;
  std::list<Type> mReleasedIDs;
};
