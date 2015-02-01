#include "NRect.hpp"


NRect::NRect() {}


NRect::NRect(const NLENGTH &left, const NLENGTH &top, const NLENGTH &right, const NLENGTH &bottom)
  : left(left), top(top), right(right), bottom(bottom) {}
