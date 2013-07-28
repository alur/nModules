//--------------------------------------------------------------------------------------
// IBrushOwner.hpp
// The nModules Project
//
// An object that contains brushes.
//
//--------------------------------------------------------------------------------------
#pragma once

#include "Brush.hpp"

class IBrushOwner {
public:
    virtual Brush *GetBrush(LPCSTR name) = 0;
};
