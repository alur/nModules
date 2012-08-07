/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Math.cpp													  August, 2012
 *	The nModules Project
 *
 *	Math functions
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include "Math.h"

/// <summary>
/// Returns the size of the overlapping area between the two rectangles.
/// </summary>
int Math::RectIntersectArea(LPRECT rect1, LPRECT rect2) {
	return max(0, min(rect1->right, rect2->right) - max(rect1->left, rect2->left))*max(0, min(rect1->bottom, rect2->bottom) - max(rect1->top, rect2->top));
}