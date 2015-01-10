//-------------------------------------------------------------------------------------------------
// /nShared/Rect.hpp
// The nModules Project
//
// A rectangle.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Distance.hpp"

#include "../Utilities/CommonD2D.h"

class Rect {
public:
  Rect();
  Rect(Distance left, Distance top, Distance right, Distance bottom);

public:
  D2D1_RECT_F Evaluate(D2D1_SIZE_F parentSize /*, float dpi */);

public:
  Distance left, top, right, bottom;
};
