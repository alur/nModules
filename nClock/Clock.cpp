/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Clock.hpp
 *  The nModules Project
 *
 *  A clock.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Clock.hpp"


/// <summary>
/// Constructor
/// </summary>
Clock::Clock(LPCTSTR clockName) : Drawable(clockName)
{
    mUse24HourDial = mSettings->GetBool(_T("24HourDial"), false);

    DrawableSettings windowDefaults;
    windowDefaults.evaluateText = true;
    windowDefaults.registerWithCore = true;

    mWindow->Initialize(&windowDefaults);

    mSecondHand.Initialize(mSettings, _T("SecondHand"), 60.0f);
    mMinuteHand.Initialize(mSettings, _T("MinuteHand"), 60.0f);
    mHourHand.Initialize(mSettings, _T("HourHand"), mUse24HourDial ? 24.0f : 12.0f);

    mWindow->AddPrePainter(&mHourHand);
    mWindow->AddPrePainter(&mMinuteHand);
    mWindow->AddPrePainter(&mSecondHand);

    UpdateHands();
    mUpdateTimer = mWindow->SetCallbackTimer(mSettings->GetInt(_T("UpdateRate"), 1000), this);

    mWindow->Show();
}


/// <summary>
/// Destructor
/// </summary>
Clock::~Clock()
{
    mWindow->ClearCallbackTimer(mUpdateTimer);
}


/// <summary>
/// Updates the rotation of the clock hands
/// </summary>
void Clock::UpdateHands()
{
    SYSTEMTIME time;
    GetLocalTime(&time);

    float second = time.wSecond + time.wMilliseconds / 1000.0f;
    float minute = time.wMinute + second / 60.0f;
    float hour = time.wHour + minute / 60.0f;

    mSecondHand.SetValue(second);
    mMinuteHand.SetValue(minute);
    mHourHand.SetValue(hour);
}


/// <summary>
/// Message handler
/// </summary>
LRESULT WINAPI Clock::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (msg)
    {
    case WM_TIMER:
        {
            if (wParam == mUpdateTimer)
            {
                UpdateHands();
                mWindow->Repaint();
            }
        }
        return 0;

    case DrawableWindow::WM_TOPPARENTLOST:
        {
            mUpdateTimer = 0;
        }
        return 0;

    case DrawableWindow::WM_NEWTOPPARENT:
        {
            mUpdateTimer = mWindow->SetCallbackTimer(mSettings->GetInt(_T("UpdateRate"), 1000), this);
        }
        return 0;
    }

    mEventHandler->HandleMessage(window, msg, wParam, lParam);
    return DefWindowProc(window, msg, wParam, lParam);
}
