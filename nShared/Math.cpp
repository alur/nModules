/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Math.cpp
 *  The nModules Project
 *
 *  Math functions
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Math.h"


/// <summary>
/// Returns the area of the rectangle.
/// </summary>
int Math::RectArea(LPRECT rect) {
    return (rect->right - rect->left)*(rect->bottom - rect->top);
}


/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int Math::RectIntersectArea(LPRECT rect1, LPRECT rect2) {
    return max(0, min(rect1->right, rect2->right) - max(rect1->left, rect2->left))*max(0, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}


/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int Math::RectUnionArea(LPRECT rect1, LPRECT rect2) {
    return RectArea(rect1) + RectArea(rect2) - RectIntersectArea(rect1, rect2);
}


/// <summary>
/// Returns the size of the area which is not within the overlap of the two rectangles.
/// </summary>
int Math::RectNonOverlappArea(LPRECT rect1, LPRECT rect2) {
    return RectArea(rect1) + RectArea(rect2) - 2*RectIntersectArea(rect1, rect2);
}
