/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Math.h
 *  The nModules Project
 *
 *  Math functions
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Math {
    int RectArea(LPRECT rect);
    int RectIntersectArea(LPRECT rect1, LPRECT rect2);
    int RectUnionArea(LPRECT rect1, LPRECT rect2);
    int RectNonOverlappArea(LPRECT rect1, LPRECT rect2);
}
