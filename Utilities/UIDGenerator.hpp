/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  UIDGenerator.hpp
 *  The nModules Project
 *
 *  Manages a set of unique ID's, letting you generate and release them.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <list>

template <class TYPE>
class UIDGenerator {
public:
  /// <summary>
  /// Constructor.
  /// </summary>
  /// <param name="start">The ID to start at.</param>
  explicit UIDGenerator(TYPE start = 0) {
    mNextFreshID = start;
  }


  /// <summary>
  /// Generates a new ID. This ID won't be generated again until it has been released.
  /// </summary>
  /// <returns>The unique ID.</returns>
  TYPE GetNewID() {
    if (mReleasedIDs.empty()) {
      return mNextFreshID++;
    }
    TYPE ret = mReleasedIDs.front();
    mReleasedIDs.pop_front();
    return ret;
  }


  /// <summary>
  /// Releases an ID, allowing it to be generated again.
  /// </summary>
  /// <param name="id">The ID to release.</param>
  void ReleaseID(TYPE id) {
    mReleasedIDs.push_back(id);
  }


private:
  TYPE mNextFreshID;
  std::list<TYPE> mReleasedIDs;
};
