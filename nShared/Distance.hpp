//-------------------------------------------------------------------------------------------------
// /nShared/Distance.hpp
// The nModules Project
//
// A distance. In the nModules, all distances are represented as a linear combination in the form
// d = a + b*parentLength + c*pixelSize, where pixelSize = DPI/96.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Utilities/Common.h"

class Distance {
public:
  Distance();
  Distance(float pixels); // TODO(Erik): Get rid of this.
  Distance(float pixels, float percent /*, float dips*/);

  Distance operator-(const Distance&);
  Distance operator+(const Distance&);
  Distance operator*(float);
  Distance operator/(float);

public:
  float Evaluate(float parentLength /*, float dpi */);
  static bool Parse(LPCWSTR distanceString, Distance &out);

private:
  float mPixels;
  float mPercent;
  float mDips;
};
