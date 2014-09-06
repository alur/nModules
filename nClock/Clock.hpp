//-------------------------------------------------------------------------------------------------
// /nClock/Clock.hpp
// The nModules Project
//
// A clock.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "ClockHand.hpp"

#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"

class Clock : public Drawable {
private:
  enum class States {
    Base,
    Count
  };

public:
  explicit Clock(LPCWSTR prefix);
  ~Clock();

public:
  void UpdateHands();

  // MessageHandler
public:
  LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) override;

private:
  StateRender<States> mStateRender;

  UINT_PTR mUpdateTimer;
  bool mUse24HourDial;

  ClockHand mSecondHand;
  ClockHand mMinuteHand;
  ClockHand mHourHand;
};
