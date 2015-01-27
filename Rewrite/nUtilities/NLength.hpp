#pragma once

/// <summary>
/// Lengths as used by the nModules are a linear function of 2 variables: the length of the parent,
/// and the pixel density of the monitor the calculation is done for. Since the DPI and parent
/// length may change frequently the result of Evaluate shouldn't be cached.
/// A DIP is 1 px at 96dpi.
/// </summary>
typedef class NLength {
public:
  NLength();
  NLength(float pixels, float percent, float dips);

public:
  NLength operator-(const NLength&);
  NLength operator+(const NLength&);
  NLength operator*(float);
  NLength operator/(float);

public:
  float Evaluate(float parentLength, float dpi);

private:
  float mPixels;
  float mPercent;
  float mDips;
} NLENGTH;
