/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IColorVal.hpp
 *  The nModules Project
 *
 *  Functions for dealing with colors.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
typedef DWORD ARGB;

class IColorVal
{
public:
    virtual ~IColorVal() {};

    // Returns true if this ColorVal does not depend on any variables.
    virtual bool IsConstant() const = 0;

    // Evaluates this ColorVal.
    virtual ARGB Evaluate() const = 0;

    // Evaluates this ColorVal.
    virtual ARGB Evaluate(ARGB DWMColor) const = 0;

    // Makes a deep copy of this ColorVal.
    // Allocd with new.
    virtual IColorVal* Copy() const = 0;
};
