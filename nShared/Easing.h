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
        SQUARED,
        CUBIC,
        QUADRATIC,
        BOUNCE,
        ELASTIC,
        SINE
    };


    float Transform(float progress, EasingType easingType);
}
