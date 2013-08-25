/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Clock.hpp
 *  The nModules Project
 *
 *  A clock.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"
#include "ClockHand.hpp"

class Clock : public Drawable
{
private:
    enum class States
    {
        Base = 0,
        Count
    };

public:
    explicit Clock(LPCWSTR prefix);
    virtual ~Clock();

    // MessageHandler
public:
    LRESULT WINAPI HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) override;

    //
public:
    void UpdateHands();

private:
    StateRender<States> mStateRender;

    UINT_PTR mUpdateTimer;
    bool mUse24HourDial;

    ClockHand mSecondHand;
    ClockHand mMinuteHand;
    ClockHand mHourHand;
};
