/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Easing.h
 *  The nModules Project
 *
 *  Functions for calculating easings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

namespace Easing {
    enum EasingType {
        LINEAR,
        INQUAD,
        OUTQUAD,
        INOUTQUAD,
        INCUBIC,
        OUTQUBIC,
        INOUTCUBIC,
        BOUNCE
    };


    float Transform(float progress, EasingType easingType);
}
