/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Easing.cpp
 *  The nModules Project
 *
 *  Functions for calculating easings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Easing.h"
#include <cmath>


/// <summary>
/// Transforms an actual progress to the "eased" progress.
/// </summary>
/// <param name="progress>How far the actual progress has gone. 0 <= progress <= 1.</param>
/// <returns>The transformed progress. Transform(0, x) == 0, Transform(1, x) == 1.</returns>
float Easing::Transform(float progress, EasingType easingType) {
    switch (easingType) {
    case LINEAR:
        return progress;
    case CUBIC:
        return progress*progress*progress;
    case SINE:
        return (float)sin(1.57079632679*progress);
    case BOUNCE:
        {
            //if (progress < 0.6) return progress*progress/0.36f;
            //if (progress < 0.8) return pow(progress-0.6, 2)
        }
    default:
        return progress;
    }
}
