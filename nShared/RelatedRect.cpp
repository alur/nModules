/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelatedRect.cpp
 *  The nModules Project
 *
 *  A rectangle made up of RelatedNumbers
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "RelatedRect.hpp"


RelatedRect::RelatedRect()
{
}


RelatedRect::RelatedRect(RelatedNumber left, RelatedNumber top, RelatedNumber right, RelatedNumber bottom)
    : left(left)
    , top(top)
    , right(right)
    , bottom(bottom)
{
}


D2D1_RECT_F RelatedRect::Evaluate(D2D1_SIZE_F related)
{
    return D2D1::RectF(
        left.Evaluate(related.width),
        top.Evaluate(related.height),
        right.Evaluate(related.width),
        bottom.Evaluate(related.height)
    );
}
