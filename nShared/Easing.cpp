/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Easing.cpp
 *  The nModules Project
 *
 *  Functions for calculating easings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Easing.h"


/// <summary>
/// Transforms an actual progress to the "eased" progress.
/// </summary>
/// <param name="progress>How far the actual progress has gone. 0 <= progress <= 1.</param>
/// <returns>The transformed progress. Transform(0, x) == 0, Transform(1, x) == 1.</returns>
float Easing::Transform(float progress, EasingType easingType) {
    switch (easingType) {
    case LINEAR:
        return progress;
    case EasingType::INOUTCUBIC:
        return progress*progress*progress;
    default:
        return progress;
    }
}
