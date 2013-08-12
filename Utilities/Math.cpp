/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Math.cpp
 *  The nModules Project
 *
 *  Math functions
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Math.h"
#include <algorithm>

using std::min;
using std::max;


/// <summary>
/// Returns the area of the rectangle.
/// </summary>
int Math::RectArea(LPRECT rect)
{
    return (rect->right - rect->left)*(rect->bottom - rect->top);
}


/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int Math::RectIntersectArea(LPRECT rect1, LPRECT rect2)
{
    return max(0L, min(rect1->right, rect2->right) - max(rect1->left, rect2->left))*max(0L, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
float Math::RectIntersectArea(D2D1_RECT_F *rect1, D2D1_RECT_F *rect2)
{
    return max(0.0f, min(rect1->right, rect2->right) - max(rect1->left, rect2->left))*max(0.0f, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}


/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
float Math::RectIntersectArea(D2D1_RECT_F rect1, D2D1_RECT_F rect2)
{
    return max(0.0f, min(rect1.right, rect2.right) - max(rect1.left, rect2.left))*max(0.0f, min(rect1.bottom, rect2.bottom) - max(rect1.top, rect2.top));
}


/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int Math::RectUnionArea(LPRECT rect1, LPRECT rect2)
{
    return RectArea(rect1) + RectArea(rect2) - RectIntersectArea(rect1, rect2);
}


/// <summary>
/// Returns the size of the area which is not within the overlap of the two rectangles.
/// </summary>
int Math::RectNonOverlappArea(LPRECT rect1, LPRECT rect2)
{
    return RectArea(rect1) + RectArea(rect2) - 2*RectIntersectArea(rect1, rect2);
}


/// <summary>
/// Performs a linear interpolation from a to b, with the specified weight
/// </summary>
float Math::Lerp(float a, float b, float weight)
{
    return a*weight + b*(1.0f - weight);
}


/// <summary>
/// Performs a linear interpolation from a to b, where the number line wraps around
/// from maxValue to minValue. e.g. when interpolating degrees.
/// </summary>
float Math::WrappingLerp(float a, float b, float weight, float minValue, float maxValue)
{
    // Assume that a and b are both within [minValue, maxValue].
    assert(a >= minValue && a <= maxValue && b >= minValue && b <= maxValue);

    // Ensure that a <= b, to simplify life
    if (a > b) {
        std::swap(a, b);
        weight = 1.0f - weight;
    }

    //
    float length = maxValue - minValue;
    float delta = b - a;
    float result;

    if (2 * delta > length)
    {
        result = Lerp(a + length, b, weight);
    }
    else
    {
        result = Lerp(a, b, weight);
    }

    // This could probably be converted to a mathematical expression.
    // Assuming W is in the [0, 1] range (which is not necesarily true), 
    // this would only happen when 2*delta > length.
    while (result > maxValue) result -= length;

    return result;
}


double Math::min(double a, double b)
{
    return std::min(a, b);
}


float Math::min(float a, float b)
{
    return std::min(a, b);
}


int Math::min(int a, int b)
{
    return std::min(a, b);
}


float Math::max(float a, float b)
{
    return std::max(a, b);
}


int Math::max(int a, int b)
{
    return std::max(a, b);
}


float Math::clamp(float a, float b, float c)
{
    return std::min(std::max(a, b), c);
}


int Math::clamp(int a, int b, int c)
{
    return std::min(std::max(a, b), c);
}
