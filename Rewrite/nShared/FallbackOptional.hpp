#pragma once

template <typename Type>
class FallbackOptional {
public:
  explicit FallbackOptional(FallbackOptional<Type> &fallback)
    : mIsSet(false), mFallback(fallback) {}

public:
  FallbackOptional &operator=(FallbackOptional<Type>&) = delete;

  void operator=(Type value) {
    mValue = value;
    mIsSet = true;
  }

public:
  inline void Clear() {
    mIsSet = false;
  }

  inline Type &Get() {
    return mIsSet ? mValue : mFallback.Get();
  }

  inline bool IsSet() {
    return mIsSet;
  }

private:
  bool mIsSet;
  Type mValue;
  FallbackOptional<Type> &mFallback;
};
