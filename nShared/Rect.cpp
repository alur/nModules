//-------------------------------------------------------------------------------------------------
// /nShared/Rect.cpp
// The nModules Project
//
// A rectangle.
//-------------------------------------------------------------------------------------------------
#include "Rect.hpp"


Rect::Rect() {}


Rect::Rect(Distance left, Distance top, Distance right, Distance bottom)
  : left(left)
  , top(top)
  , right(right)
  , bottom(bottom) {}


D2D1_RECT_F Rect::Evaluate(D2D1_SIZE_F parentSize /*, float dpi*/) {
  return D2D1::RectF(
    left.Evaluate(parentSize.width /*, dpi */),
    top.Evaluate(parentSize.height /*, dpi */),
    right.Evaluate(parentSize.width /*, dpi */),
    bottom.Evaluate(parentSize.height /*, dpi */));
}
