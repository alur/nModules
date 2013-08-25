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
TaskButton::TaskButton(Drawable* parent, HWND watchedWindow, ButtonSettings &btnSettings)
    : Drawable(parent, _T("Button"))
    , mButtonSettings(btnSettings)
{

    mWatchedWindow = watchedWindow;

    mWindow->Initialize(mButtonSettings.mWindowSettings, &mButtonSettings.mStateRender);

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
        ActivateState(State::Minimized);
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
    if (mButtonSettings.mNoIcons)
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
        mIcon = mWindow->AddOverlay(mButtonSettings.mIconRect, icon, 10);
    }
    mWindow->Repaint();
}


/// <summary>
/// Sets the overlay icon of this button.
/// </summary>
void TaskButton::SetOverlayIcon(HICON overlayIcon)
{
    if (mButtonSettings.mNoIcons)
    {
        return;
    }
    
    mWindow->RemoveOverlay(mIconOverlay);
    mIconOverlay.mValid = false;
    if (overlayIcon != nullptr)
    {
        mIconOverlay = mWindow->AddOverlay(mButtonSettings.mOverlayIconRect, overlayIcon, 20);
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
    ActivateState(State::Active);
    ClearState(State::Minimized);

    if (mIsFlashing)
    {
        ClearState(State::Flashing);
        mWindow->ClearCallbackTimer(mFlashTimer);
        mIsFlashing = false;
    }
}

//
///// <summary>
///// Activates or deactivates the flashing state.
///// </summary>
//void TaskButton::SetFlashingState(bool value)
//{
//    if (value != mStates[State::Flashing]->active)
//    {
//        if (value)
//        {
//            if (mStates[State::Hover]->active && mStates[State::Minimized]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
//            }
//            if (mStates[State::Minimized]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedFlashing]);
//            }
//            if (mStates[State::Hover]->active)
//            {
//                mWindow->ActivateState(mStates[State::FlashingHover]);
//            }
//            mWindow->ActivateState(mStates[State::Flashing]);
//        }
//        else
//        {
//            mWindow->ClearState(mStates[State::Flashing]);
//            mWindow->ClearState(mStates[State::MinimizedFlashing]);
//            mWindow->ClearState(mStates[State::FlashingHover]);
//            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
//        }
//    }
//}
//
//
///// <summary>
///// Activates or deactivates the active state.
///// </summary>
//void TaskButton::SetActiveState(bool value)
//{
//    if (value != mStates[State::Active]->active)
//    {
//        if (value)
//        {
//            if (mStates[State::Hover]->active)
//            {
//                mWindow->ActivateState(mStates[State::ActiveHover]);
//            }
//            mWindow->ActivateState(mStates[State::Active]);
//        }
//        else
//        {
//            mWindow->ClearState(mStates[State::Active]);
//            mWindow->ClearState(mStates[State::ActiveHover]);
//        }
//    }
//}
//
//
///// <summary>
///// Activates or deactivates the hover state.
///// </summary>
//void TaskButton::SetHoverState(bool value)
//{
//    if (value != mStates[State::Hover]->active)
//    {
//        if (value)
//        {
//            if (mStates[State::Minimized]->active && mStates[State::Flashing]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
//            }
//            if (mStates[State::Flashing]->active)
//            {
//                mWindow->ActivateState(mStates[State::FlashingHover]);
//            }
//            if (mStates[State::Active]->active)
//            {
//                mWindow->ActivateState(mStates[State::ActiveHover]);
//            }
//            if (mStates[State::Minimized]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedHover]);
//            }
//            mWindow->ActivateState(mStates[State::Hover]);
//        }
//        else
//        {
//            mWindow->ClearState(mStates[State::Hover]);
//            mWindow->ClearState(mStates[State::MinimizedHover]);
//            mWindow->ClearState(mStates[State::ActiveHover]);
//            mWindow->ClearState(mStates[State::FlashingHover]);
//            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
//        }
//    }
//}
//
//
///// <summary>
///// Activates or deactivates the minimized state.
///// </summary>
//void TaskButton::SetMinmizedState(bool value)
//{
//    if (value != mStates[State::Minimized]->active)
//    {
//        if (value)
//        {
//            if (mStates[State::Hover]->active && mStates[State::Flashing]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
//            }
//            if (mStates[State::Hover]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedHover]);
//            }
//            if (mStates[State::Flashing]->active)
//            {
//                mWindow->ActivateState(mStates[State::MinimizedFlashing]);
//            }
//            mWindow->ActivateState(mStates[State::Minimized]);
//        }
//        else
//        {
//            mWindow->ClearState(mStates[State::Minimized]);
//            mWindow->ClearState(mStates[State::MinimizedFlashing]);
//            mWindow->ClearState(mStates[State::MinimizedHover]);
//            mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
//        }
//    }
//}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate()
{
    ClearState(State::Active);
}


/// <summary>
/// Tells this button to start flashing.
/// </summary>
void TaskButton::Flash()
{
    if (!mIsFlashing)
    {
        mIsFlashing = true;
        ActivateState(State::Flashing);
        mFlashTimer = mWindow->SetCallbackTimer(mButtonSettings.mFlashInterval, this);
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


void TaskButton::ActivateState(State state)
{
    mButtonSettings.mStateRender.ActivateState(state, mWindow);
}


void TaskButton::ClearState(State state)
{
    mButtonSettings.mStateRender.ClearState(state, mWindow);
}


void TaskButton::ToggleState(State state)
{
    mButtonSettings.mStateRender.ToggleState(state, mWindow);
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
                ActivateState(State::Minimized);
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
                ActivateState(State::Hover);

                RECT r;
                mWindow->GetScreenRect(&r);
                ((Taskbar*)mParent)->ShowThumbnail(mWatchedWindow, &r);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mMouseIsOver = false;
            ClearState(State::Hover);
            ((Taskbar*)mParent)->HideThumbnail();
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == mFlashTimer)
            {
                if (mIsFlashing)
                {
                    ToggleState(State::Flashing);
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
