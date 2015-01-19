//-------------------------------------------------------------------------------------------------
// /nShared/Easing.cpp
// The nModules Project
//
// Functions for calculating easings.
//-------------------------------------------------------------------------------------------------
#include "Easing.h"

#include "../Utilities/AlgorithmExtension.h"
#include "../Utilities/StringUtils.h"

#include <cmath>


/// <summary>
/// Easing Name -> Easing::Type
/// </summary>
static StringKeyedMaps<LPCWSTR, Easing::Type>::ConstUnorderedMap sStringToEasing({
  { L"Cubic", Easing::Type::Cubic },
  { L"Sine", Easing::Type::Sine },
  { L"Bounce", Easing::Type::Bounce },
  { L"Linear", Easing::Type::Linear }
});


/// <summary>
/// Transforms an actual progress to the "eased" progress.
/// </summary>
/// <param name="progress>How far the actual progress has gone. 0 <= progress <= 1.</param>
/// <returns>The transformed progress. Transform(0, x) == 0, Transform(1, x) == 1.</returns>
float Easing::Transform(float progress, Type easingType) {
  switch (easingType) {
  case Type::Linear:
    return progress;
  case Type::Cubic:
    return progress*progress*progress;
  case Type::Sine:
    return (float)sin(1.57079632679*progress);
  case Type::Bounce:
    //if (progress < 0.6) return progress*progress/0.36f;
    //if (progress < 0.8) return pow(progress-0.6, 2)
  default:
    return progress;
  }
}


/// <summary>
/// Parses a string into an easing.
/// </summary>
Easing::Type Easing::EasingFromString(LPCWSTR str) {
  return std::get(sStringToEasing, str, Type::Linear);
}
