/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  UIDGenerator.cpp
 *  The nModules Project
 *
 *  Manages a set of unique ID's, letting you generate and release them.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "UIDGenerator.hpp"
#include "Common.h"


/// <summary>
/// Constructor.
/// </summary>
/// <param name="start">The ID to start at.</param>
template <class TYPE>
UIDGenerator<TYPE>::UIDGenerator(TYPE start) {
    this->nextFreshID = start;
}


/// <summary>
/// Destructor.
/// </summary>
template <class TYPE>
UIDGenerator<TYPE>::~UIDGenerator() {
    this->releasedIDs.clear();
}


/// <summary>
/// Generates a new ID. This ID won't be generated again until it has been released.
/// </summary>
/// <returns>The unique ID.</returns>
template <class TYPE>
TYPE UIDGenerator<TYPE>::GetNewID() {
    if (this->releasedIDs.empty()) {
        return this->nextFreshID++;
    }
    TYPE ret = this->releasedIDs.front();
    this->releasedIDs.pop_front();
    return ret;
}


/// <summary>
/// Releases an ID, allowing it to be generated again.
/// </summary>
/// <param name="id">The ID to release.</param>
template<class TYPE>
void UIDGenerator<TYPE>::ReleaseID(TYPE id) {
    this->releasedIDs.push_back(id);
}


/// <summary>
/// Forces the compiler to generate functions for the types we want.
/// </summary>
void Generator() {
    UIDGenerator<UINT_PTR> u;
    u.ReleaseID(u.GetNewID());
    UIDGenerator<UINT> u2;
    u2.ReleaseID(u2.GetNewID());
}
