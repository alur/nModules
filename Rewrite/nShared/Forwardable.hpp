#pragma once

/// <summary>
/// A class which either holds a value or acts as a reference.
/// </summary>
template <typename T> class Forwardable {
public:
  Forwardable(T *reference) {}

public:
  T &Get() {
    return mValue;
  }

  bool IsSet() {
    return mIsSet;
  }

private:
  T mValue;
  Forwardable<T> mFallback;
  bool mIsSet;
};
