//-------------------------------------------------------------------------------------------------
// /Utilities/Math.cpp
// The nModules Project
//
// Math functions.
//-------------------------------------------------------------------------------------------------
#include "Math.h"

#include <algorithm>

using std::min;
using std::max;


int RectArea(LPCRECT rect) {
  return (rect->right - rect->left) * (rect->bottom - rect->top);
}


int RectIntersectArea(LPCRECT rect1, LPCRECT rect2) {
  return max(0L, min(rect1->right, rect2->right) - max(rect1->left, rect2->left)) *
      max(0L, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}


float RectIntersectArea(D2D1_RECT_F *rect1, D2D1_RECT_F *rect2) {
  return max(0.0f, min(rect1->right, rect2->right) - max(rect1->left, rect2->left)) *
      max(0.0f, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}


float RectIntersectArea(D2D1_RECT_F rect1, D2D1_RECT_F rect2) {
  return max(0.0f, min(rect1.right, rect2.right) - max(rect1.left, rect2.left)) *
      max(0.0f, min(rect1.bottom, rect2.bottom) - max(rect1.top, rect2.top));
}


int RectUnionArea(LPCRECT rect1, LPCRECT rect2) {
  return RectArea(rect1) + RectArea(rect2) - RectIntersectArea(rect1, rect2);
}


int RectNonOverlapArea(LPCRECT rect1, LPCRECT rect2) {
  return RectArea(rect1) + RectArea(rect2) - 2 * RectIntersectArea(rect1, rect2);
}


float Lerp(float a, float b, float weight) {
  return a * weight + b * (1.0f - weight);
}


float WrappingLerp(float a, float b, float weight, float minValue, float maxValue) {
  // Assume that a and b are both within [minValue, maxValue].
  ASSERT(a >= minValue && a <= maxValue && b >= minValue && b <= maxValue);

  // Ensure that a <= b, to simplify life
  if (a > b) {
    std::swap(a, b);
    weight = 1.0f - weight;
  }

  float length = maxValue - minValue;
  float delta = b - a;
  float result;

  if (2 * delta > length) {
    result = Lerp(a + length, b, weight);
  } else {
    result = Lerp(a, b, weight);
  }

  // This could probably be converted to a mathematical expression. Assuming W is in the [0, 1]
  // range (which is not necesarily true), this would only happen when 2*delta > length.
  while (result > maxValue) result -= length;

  return result;
}


float Clamp(float a, float b, float c) {
  return min(max(a, b), c);
}


int Clamp(int a, int b, int c) {
  return min(max(a, b), c);
}
