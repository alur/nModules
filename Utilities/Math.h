/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Math.h
 *  The nModules Project
 *
 *  Math functions
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Common.h"
#include <d2d1.h>
#include <algorithm>

#define CLAMP(a, b, c) std::min(std::max(a, b), c)

namespace Math {
  int RectArea(LPCRECT rect);
  int RectIntersectArea(LPCRECT rect1, LPCRECT rect2);
  int RectUnionArea(LPCRECT rect1, LPCRECT rect2);
  int RectNonOverlappArea(LPCRECT rect1, LPCRECT rect2);
    
  float RectIntersectArea(D2D1_RECT_F *rect1, D2D1_RECT_F *rect2);
  float RectIntersectArea(D2D1_RECT_F rect1, D2D1_RECT_F rect2);

  float Lerp(float a, float b, float weight);
  float WrappingLerp(float a, float b, float weight, float minValue, float maxValue);

  //
  double min(double a, double b);
  float min(float a, float b);
  int min(int a, int b);
  float max(float a, float b);
  int max(int a, int b);
  float clamp(float a, float b, float c);
  int clamp(int a, int b, int c);
}
