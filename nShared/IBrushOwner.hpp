//--------------------------------------------------------------------------------------
// IBrushOwner.hpp
// The nModules Project
//
// An object that contains brushes.
//
//--------------------------------------------------------------------------------------
#pragma once

#include "Brush.hpp"

class IBrushOwner
{
public:
    virtual Brush *GetBrush(LPCTSTR name) = 0;
};
