#include "NLength.hpp"


NLength::NLength() {}


NLength::NLength(float pixels, float percent, float dips)
    : mPixels(pixels), mPercent(percent), mDips(dips) {}


float NLength::Evaluate(float parentNLength, float dpi) {
  return mPixels + parentNLength * mPercent + mDips * dpi / 96.0f;
}


NLength NLength::operator-(const NLength &other) {
  return NLength(mPixels - other.mPixels, mPercent - other.mPercent, mDips - other.mDips);
}


NLength NLength::operator+(const NLength &other) {
  return NLength(mPixels + other.mPixels, mPercent + other.mPercent, mDips + other.mDips);
}


NLength NLength::operator*(float factor) {
  return NLength(mPixels * factor, mPercent * factor, mDips * factor);
}


NLength NLength::operator/(float factor) {
  return NLength(mPixels / factor, mPercent / factor, mDips / factor);
}
