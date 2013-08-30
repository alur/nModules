/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  RelatedRect.hpp
 *  The nModules Project
 *
 *  A rectangle made up of RelatedNumbers
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include <d2d1.h>
#include "RelatedNumber.hpp"

class RelatedRect
{
    // Constructor
public:
    RelatedRect();
    RelatedRect(RelatedNumber left, RelatedNumber top, RelatedNumber right, RelatedNumber bottom);

    //
public:
    D2D1_RECT_F Evaluate(D2D1_SIZE_F related);

    // Member variables
public:
    RelatedNumber left, top, right, bottom;
};
