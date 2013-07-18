/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Math.h
 *  The nModules Project
 *
 *  Math functions
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>

#define clamp(a,b,c) min(max((a),(b)), (c))

namespace Math {
    int RectArea(LPRECT rect);
    int RectIntersectArea(LPRECT rect1, LPRECT rect2);
    int RectUnionArea(LPRECT rect1, LPRECT rect2);
    int RectNonOverlappArea(LPRECT rect1, LPRECT rect2);

    float RectIntersectArea(D2D1_RECT_F &rect1, D2D1_RECT_F &rect2);

}
