/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  UIDGenerator.hpp
 *  The nModules Project
 *
 *  Manages a set of unique ID's, letting you generate and release them.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <list>

using std::list;

template <class TYPE>
class UIDGenerator {
public:
    explicit UIDGenerator(TYPE start = 0);
    virtual ~UIDGenerator();

    TYPE GetNewID();
    void ReleaseID(TYPE id);

private:
    TYPE nextFreshID;
    list<TYPE> releasedIDs;
};
