//-------------------------------------------------------------------------------------------------
// /nShared/Easing.h
// The nModules Project
//
// Functions for calculating easings.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Utilities/Common.h"

namespace Easing {
  enum class Type {
    Linear,
    Squared,
    Cubic,
    Quadractic,
    Bounce,
    Elastic,
    Sine
  };

  float Transform(float progress, Type easingType);
  Type EasingFromString(LPCWSTR str);
}
