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
  NLength(float pixels, float fraction, float dips);

public:
  NLength &operator-=(const NLength&);
  NLength &operator+=(const NLength&);
  NLength operator-(const NLength&) const;
  NLength operator+(const NLength&) const;
  NLength operator*(float) const;
  NLength operator/(float) const;
  bool operator==(const NLength&) const;
  bool operator!=(const NLength&) const;

public:
  float Evaluate(float parentLength, float dpi) const;

private:
  float mPixels;
  float mFraction;
  float mDips;
} NLENGTH;


typedef struct NRect {
public:
  NRect();
  NRect(const NLENGTH &left, const NLENGTH &top, const NLENGTH &right, const NLENGTH &bottom);

public:
  NLENGTH left;
  NLENGTH top;
  NLENGTH right;
  NLENGTH bottom;
} NRECT, *PNRECT;
typedef const NRECT *LPCNRECT;


typedef struct NSize {
public:
  NSize();
  NSize(const NLENGTH &width, const NLENGTH &height);

public:
  NLENGTH width;
  NLENGTH height;
} NSIZE;


typedef struct NPoint {
public:
  NPoint();
  NPoint(const NLENGTH &x, const NLENGTH &y);

public:
  NLENGTH x;
  NLENGTH y;
} NPOINT;
