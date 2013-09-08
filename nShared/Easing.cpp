/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Easing.cpp
 *  The nModules Project
 *
 *  Functions for calculating easings.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Easing.h"
#include <cmath>
#include <tchar.h>


/// <summary>
/// Transforms an actual progress to the "eased" progress.
/// </summary>
/// <param name="progress>How far the actual progress has gone. 0 <= progress <= 1.</param>
/// <returns>The transformed progress. Transform(0, x) == 0, Transform(1, x) == 1.</returns>
float Easing::Transform(float progress, Type easingType)
{
    switch (easingType)
    {
    case Type::Linear:
        return progress;
    case Type::Cubic:
        return progress*progress*progress;
    case Type::Sine:
        return (float)sin(1.57079632679*progress);
    case Type::Bounce:
        {
            //if (progress < 0.6) return progress*progress/0.36f;
            //if (progress < 0.8) return pow(progress-0.6, 2)
        }
    default:
        return progress;
    }
}

 
/// <summary>
/// Parses a string into an easing.
/// </summary>
Easing::Type Easing::EasingFromString(LPCTSTR str)
{
    if (_tcsicmp(str, _T("Cubic"))) return Type::Cubic;
    if (_tcsicmp(str, _T("Sine"))) return Type::Sine;
    if (_tcsicmp(str, _T("Bounce"))) return Type::Bounce;
    return Type::Linear;
}
