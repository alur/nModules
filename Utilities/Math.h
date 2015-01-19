//-------------------------------------------------------------------------------------------------
// /Utilities/Math.h
// The nModules Project
//
// Math functions.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "CommonD2D.h"

/// <summary>
/// Returns the area of the rectangle.
/// </summary>
int RectArea(LPCRECT rect);

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int RectIntersectArea(LPCRECT rect1, LPCRECT rect2);
float RectIntersectArea(D2D1_RECT_F *rect1, D2D1_RECT_F *rect2);
float RectIntersectArea(D2D1_RECT_F rect1, D2D1_RECT_F rect2);

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int RectUnionArea(LPCRECT rect1, LPCRECT rect2);

/// <summary>
/// Returns the size of the area which is not within the overlap of the two rectangles.
/// </summary>
int RectNonOverlapArea(LPCRECT rect1, LPCRECT rect2);

/// <summary>
/// Performs a linear interpolation from a to b, with the specified weight
/// </summary>
float Lerp(float a, float b, float weight);

/// <summary>
/// Performs a linear interpolation from a to b, where the number line wraps around
/// from maxValue to minValue. e.g. when interpolating degrees.
/// </summary>
float WrappingLerp(float a, float b, float weight, float minValue, float maxValue);

float Clamp(float a, float b, float c);
int Clamp(int a, int b, int c);
