#pragma once

#include "../Headers/d2d1.h"

/// <summary>
/// Returns the area of the rectangle.
/// </summary>
int RectArea(LPCRECT);

/// <summary>
/// Gets the rectangle representing the intersection of the two rectangles. Returns true iff the
/// rectangles intersect.
/// </summary>
bool RectIntersection(LPCRECT a, LPCRECT b, LPRECT out);
bool RectIntersection(const D2D1_RECT_F *a, const D2D1_RECT_F *b, D2D1_RECT_F *out);

/// <summary>
/// Returns whether or not the 2 rectangles overlap.
/// </summary>
bool RectIntersects(LPCRECT, LPCRECT);
bool RectIntersects(const D2D1_RECT_F*, const D2D1_RECT_F*);

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int RectIntersectArea(LPCRECT, LPCRECT);
float RectIntersectArea(const D2D1_RECT_F*, const D2D1_RECT_F*);
float RectIntersectArea(D2D1_RECT_F, D2D1_RECT_F);

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int RectUnionArea(LPCRECT, LPCRECT);

/// <summary>
/// Returns the size of the area which is not within the overlap of the two rectangles.
/// </summary>
int RectNonOverlapArea(LPCRECT, LPCRECT);

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
