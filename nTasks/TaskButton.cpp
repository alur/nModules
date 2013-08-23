/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.cpp
 *  The nModules Project
 *
 *  Implementation of the TaskButton class. Represents a taskbar button.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "../nShared/LSModule.hpp"
#include "Taskbar.hpp"
#include "TaskButton.hpp"
#include <shellapi.h>


/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(Drawable* parent, HWND watchedWindow) : Drawable(parent, _T("Button"))
{
    //
    mIconSettings = mSettings->CreateChild(_T("Icon"));
    LoadSettings();

    mWatchedWindow = watchedWindow;

    //
    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = Color::Create(0x00000000);
    stateDefaults.textOffsetLeft = 36;
    stateDefaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

    mWindow->Initialize(nullptr, &stateDefaults);

    // Add states to the window
    mStates[State::Minimized] = mWindow->AddState(_T("Minimized"), 25, &stateDefaults);
    mStates[State::Flashing] = mWindow->AddState(_T("Flashing"), 50, &stateDefaults);
    mStates[State::MinimizedFlashing] = mWindow->AddState(_T("MinimizedFlashing"), 75, &stateDefaults, &mStates[State::Flashing]);
    mStates[State::Active] = mWindow->AddState(_T("Active"), 100, &stateDefaults);
    mStates[State::Hover] = mWindow->AddState(_T("Hover"), 125, &stateDefaults);
    mStates[State::MinimizedHover] = mWindow->AddState(_T("MinimizedHover"), 150, &stateDefaults, &mStates[State::Hover]);
    mStates[State::ActiveHover] = mWindow->AddState(_T("ActiveHover"), 175, &stateDefaults, &mStates[State::Active]);
    mStates[State::FlashingHover] = mWindow->AddState(_T("FlashingHover"), 200, &stateDefaults, &mStates[State::Flashing]);
    mStates[State::MinimizedFlashingHover] = mWindow->AddState(_T("MinimizedFlashingHover"), 225, &stateDefaults, &mStates[State::FlashingHover]);

    // Initalize variables
    mMouseIsOver = false;
    mIsFlashing = false;

    // Reset the system menu for the window
    mMenu = GetSystemMenu(mWatchedWindow, FALSE);
    if (!IsMenu(mMenu))
    {
        GetSystemMenu(mWatchedWindow, TRUE);
        mMenu = GetSystemMenu(mWatchedWindow, FALSE);
    }

    // Check if we're minimized
    if (IsIconic(watchedWindow))
    {
        SetMinmizedState(true);
    }
}


/// <summary>
/// Destructor
/// </summary>
TaskButton::~TaskButton()
{
    if (mIsFlashing)
    {
        mWindow->ClearCallbackTimer(mFlashTimer);
    }
}


/// <summary>
/// Sets the icon of this button.
/// </summary>
void TaskButton::SetIcon(HICON icon)
{
    if (mNoIcons)
    {
        return;
    }

    mWindow->RemoveOverlay(mIcon);
    mIcon.mValid = false;
    
    if (icon == nullptr)
    {
        icon = LoadIcon(NULL, IDI_APPLICATION);
    }
    if (icon != nullptr)
    {
        mIcon = mWindow->AddOverlay(mIconRect, icon, 10);
    }
    mWindow->Repaint();
}


/// <summary>
/// Sets the overlay icon of this button.
/// </summary>
void TaskButton::SetOverlayIcon(HICON overlayIcon)
{
    if (mNoIcons)
    {
        return;
    }
    
    mWindow->RemoveOverlay(mIconOverlay);
    mIconOverlay.mValid = false;
    if (overlayIcon != nullptr)
    {
        mIconOverlay = mWindow->AddOverlay(mOverlayIconRect, overlayIcon, 20);
    }
    mWindow->Repaint();
}


/// <summary>
/// Sets the text of this button.
/// </summary>
void TaskButton::SetText(LPCWSTR title)
{
    mWindow->SetText(title);
    mWindow->Repaint();
}


/// <summary>
/// Sets the progress state of this button.
/// </summary>
void TaskButton::SetProgressState(TBPFLAG /* state */)
{
}


/// <summary>
/// Sets the progress value of this button.
/// </summary>
void TaskButton::SetProgressValue(USHORT /* progress */)
{
}


/// <summary>
/// Loads RC settings for task buttons.
/// </summary>
void TaskButton::LoadSettings(bool /* bIsRefresh */)
{
    mUseFlashing = mSettings->GetBool(_T("UseFlashing"), true);
    mFlashInterval = mSettings->GetInt(_T("FlashInterval"), 500);
    mNoIcons = mSettings->GetBool(_T("NoIcons"), false);

    float iconSize = mIconSettings->GetFloat(_T("Size"), 32);
    float iconX = mIconSettings->GetFloat(_T("X"), 2);
    float iconY = mIconSettings->GetFloat(_T("Y"), 2);
    mIconRect = D2D1::RectF(iconX, iconY, iconX + iconSize, iconY + iconSize);

    float overlayIconSize = mIconSettings->GetFloat(_T("OverlaySize"), 16);
    float overlayIconOffsetX = mIconSettings->GetFloat(_T("OverlayOffsetX"), 1);
    float overlayIconOffsetY = mIconSettings->GetFloat(_T("OverlayOffsetY"), 1);

    mOverlayIconRect = D2D1::RectF(
        iconX + iconSize - overlayIconSize + overlayIconOffsetX,
        iconY + iconSize - overlayIconSize + overlayIconOffsetY,
        iconX + iconSize + overlayIconOffsetX,
        iconY + iconSize + overlayIconOffsetY);
}


/// <summary>
/// Moves and resizes the taaskbutton.
/// </summary>
void TaskButton::Reposition(UINT x, UINT y, UINT width, UINT height)
{
    mWindow->SetPosition(x, y, width, height);
}


/// <summary>
/// Activates this button.
/// </summary>
void TaskButton::Activate()
{
    SetActiveState(true);
    
    if (mStates[State::Minimized]->active)
    {
        SetMinmizedState(false);
    }

    if (mIsFlashing)
    {
        SetFlashingState(false);
        mFlashOn = false;
        mWindow->ClearCallbackTimer(mFlashTimer);
        mIsFlashing = false;
    }
}


/// <summary>
/// Activates or deactivates the flashing state.
/// </summary>
void TaskButton::SetFlashingState(bool value)
{
    if (value != mStates[State::Flashing]->active)
    {
        if (value)
        {
            if (mStates[State::Hover]->active && mStates[State::Minimized]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
            }
            if (mStates[State::Minimized]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedFlashing]);
            }
            if (mStates[State::Hover]->active)
            {
                mWindow->ActivateState(mStates[State::FlashingHover]);
            }
            mWindow->ActivateState(mStates[State::Flashing]);
        }
        else
        {
            mWindow->ClearState(mStates[State::Flashing]);
            mWindow->ClearState(mStates[State::MinimizedFlashing]);
            mWindow->ClearState(mStates[State::FlashingHover]);
            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
        }
    }
}


/// <summary>
/// Activates or deactivates the active state.
/// </summary>
void TaskButton::SetActiveState(bool value)
{
    if (value != mStates[State::Active]->active)
    {
        if (value)
        {
            if (mStates[State::Hover]->active)
            {
                mWindow->ActivateState(mStates[State::ActiveHover]);
            }
            mWindow->ActivateState(mStates[State::Active]);
        }
        else
        {
            mWindow->ClearState(mStates[State::Active]);
            mWindow->ClearState(mStates[State::ActiveHover]);
        }
    }
}


/// <summary>
/// Activates or deactivates the hover state.
/// </summary>
void TaskButton::SetHoverState(bool value)
{
    if (value != mStates[State::Hover]->active)
    {
        if (value)
        {
            if (mStates[State::Minimized]->active && mStates[State::Flashing]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
            }
            if (mStates[State::Flashing]->active)
            {
                mWindow->ActivateState(mStates[State::FlashingHover]);
            }
            if (mStates[State::Active]->active)
            {
                mWindow->ActivateState(mStates[State::ActiveHover]);
            }
            if (mStates[State::Minimized]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedHover]);
            }
            mWindow->ActivateState(mStates[State::Hover]);
        }
        else
        {
            mWindow->ClearState(mStates[State::Hover]);
            mWindow->ClearState(mStates[State::MinimizedHover]);
            mWindow->ClearState(mStates[State::ActiveHover]);
            mWindow->ClearState(mStates[State::FlashingHover]);
            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
        }
    }
}


/// <summary>
/// Activates or deactivates the minimized state.
/// </summary>
void TaskButton::SetMinmizedState(bool value)
{
    if (value != mStates[State::Minimized]->active)
    {
        if (value)
        {
            if (mStates[State::Hover]->active && mStates[State::Flashing]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
            }
            if (mStates[State::Hover]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedHover]);
            }
            if (mStates[State::Flashing]->active)
            {
                mWindow->ActivateState(mStates[State::MinimizedFlashing]);
            }
            mWindow->ActivateState(mStates[State::Minimized]);
        }
        else
        {
            mWindow->ClearState(mStates[State::Minimized]);
            mWindow->ClearState(mStates[State::MinimizedFlashing]);
            mWindow->ClearState(mStates[State::MinimizedHover]);
            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
        }
    }
}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate()
{
    SetActiveState(false);
}


/// <summary>
/// Tells this button to start flashing.
/// </summary>
void TaskButton::Flash()
{
    if (!mIsFlashing)
    {
        mIsFlashing = true;
        mFlashOn = true;
        SetFlashingState(true);
        mFlashTimer = mWindow->SetCallbackTimer(mFlashInterval, this);
    }
}


/// <summary>
/// Shows this button.
/// </summary>
void TaskButton::Show()
{
    mWindow->Show();
}


/// <summary>
/// Shows the context menu for this task button.
/// </summary>
void TaskButton::ShowMenu()
{
    WINDOWPLACEMENT wp;

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(mWatchedWindow, &wp);

    // Select which menu items are enabled
    EnableMenuItem(mMenu, SC_RESTORE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_SIZE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MINIMIZE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(mMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // let application modify menu
    PostMessage(mWatchedWindow, WM_INITMENU, (WPARAM)mMenu, 0);
    PostMessage(mWatchedWindow, WM_INITMENUPOPUP, (WPARAM)mMenu, MAKELPARAM(0, TRUE));
    
    POINT pt;
    GetCursorPos(&pt);
    
    int command = TrackPopupMenu(mMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, mWindow->GetWindowHandle(), NULL);
    if (command != 0)
    {
        PostMessage(mWatchedWindow, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
}


/// <summary>
/// Places the rect of this button into the 2 POINTS structures pointed to by lpPoints.
/// </summary>
void TaskButton::GetMinRect(LPPOINTS points)
{
    RECT r;
    mWindow->GetScreenRect(&r);
    points[0].x = (short)r.left;
    points[0].y = (short)r.top;
    points[1].x = (short)r.right;
    points[1].y = (short)r.bottom;
}


/// <summary>
/// Handles window messages for this button.
/// </summary>
LRESULT WINAPI TaskButton::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (message)
    {
    case WM_LBUTTONUP:
        {
            if (IsIconic(mWatchedWindow))
            {
                BringWindowToTop(mWatchedWindow);
                PostMessage(mWatchedWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
                SetForegroundWindow(mWatchedWindow);
            }
            else if (GetForegroundWindow() == mWatchedWindow)
            {
                PostMessage(mWatchedWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                SetMinmizedState(true);
            }
            else
            {
                SetForegroundWindow(mWatchedWindow);
            }
            ((Taskbar*)mParent)->HideThumbnail();
        }
        return 0;

    case WM_RBUTTONUP:
        {
            SetWindowPos(mWatchedWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            ShowMenu();
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            if (!mMouseIsOver)
            {
                mMouseIsOver = true;
                SetHoverState(true);

                RECT r;
                mWindow->GetScreenRect(&r);
                ((Taskbar*)mParent)->ShowThumbnail(mWatchedWindow, &r);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mMouseIsOver = false;
            SetHoverState(false);
            ((Taskbar*)mParent)->HideThumbnail();
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == mFlashTimer)
            {
                if (mIsFlashing)
                {
                    mFlashOn = !mFlashOn;
                    SetFlashingState(mFlashOn);
                }
                else
                {
                    mWindow->ClearCallbackTimer(mFlashTimer);
                }
            }
        }
        return 0;

    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}
