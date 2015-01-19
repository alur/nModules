//-------------------------------------------------------------------------------------------------
// /nShared/Distance.cpp
// The nModules Project
//
// A distance. In the nModules, all distances are represented as a linear combination in the form
// d = a + b*parentLength + c*pixelSize, where pixelSize = DPI/96.
//-------------------------------------------------------------------------------------------------
#include "Distance.hpp"

#include <stdlib.h>


Distance::Distance() {}


Distance::Distance(float pixels)
  : mPixels(pixels)
  , mPercent(0)
  , mDips(0) {}


Distance::Distance(float pixels, float percent /*, float dips */)
  : mPixels(pixels)
  , mPercent(percent)
  , mDips(0) {}


/// <summary>
/// Subtraction operator
/// </summary>
Distance Distance::operator-(const Distance &other) {
  return Distance(mPixels - other.mPixels, mPercent - other.mPercent /*, mDips - other.mDips */);
}


/// <summary>
/// Addition operator
/// </summary>
Distance Distance::operator+(const Distance &other) {
  return Distance(mPixels + other.mPixels, mPercent + other.mPercent /*, mDips + other.mDips */);
}


/// <summary>
/// Multiplication operator
/// </summary>
Distance Distance::operator*(float factor) {
  return Distance(mPixels * factor, mPercent * factor /*, mDips * factor*/);
}


/// <summary>
/// Multiplication operator
/// </summary>
Distance Distance::operator/(float factor) {
  return Distance(mPixels / factor, mPercent / factor /*, mDips / factor*/);
}


/// <summary>
/// Returns the effective number of pixels.
/// </summary>
float Distance::Evaluate(float parentLength /*, float dpi*/) {
  return mPixels + parentLength * mPercent /* + dpi / 96.0f * mDips */;
}


/// <summary>
/// Parses a Distance from a string.
/// </summary>
/// <param name="distanceString">The string to parse.</param>
/// <param name="result">
/// If this is not null, and the function returns true, this will be set to the parsed related
/// number.
/// </param>
/// <return>true if the string is valid related number.</return>
bool Distance::Parse(LPCWSTR distanceString, Distance &out) {
  if (distanceString == nullptr || *distanceString == L'\0') {
    return false;
  }

  float pixels = 0;
  float percentage = 0;
  float dips = 0;

  // Look at the string as a collection of tokens, delimited by + and -
  while (*distanceString) {
    float number = wcstof(distanceString, const_cast<LPWSTR*>(&distanceString));
    if (*distanceString == L'%') {
      percentage += number;
      ++distanceString;
    } else if (distanceString[0] == L'd' && distanceString[1] == L'i' && distanceString[2] == L'p') {
      dips += number;
      distanceString += 3;
    } else {
      pixels += number;
    }

    // The next token has to be a +, -, or end of string.
    if (*distanceString != L'+' && *distanceString != L'-' && *distanceString != L'\0') {
      return false;
    }
  }

  out = Distance(pixels, percentage / 100.0f /*, dips */);

  return true;
}
