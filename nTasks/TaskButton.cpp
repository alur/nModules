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


/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(Drawable* parent, HWND watchedWindow) : Drawable(parent, _T("Button")) {
    //
    this->iconSettings = mSettings->CreateChild(_T("Icon"));
    LoadSettings();

    this->watchedWindow = watchedWindow;

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
    this->mouseIsOver = false;
    this->isFlashing = false;

    // Reset the system menu for the window
    this->menu = GetSystemMenu(this->watchedWindow, FALSE);
    if (!IsMenu(this->menu)) {
        GetSystemMenu(this->watchedWindow, TRUE);
        this->menu = GetSystemMenu(this->watchedWindow, FALSE);
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
    if (this->isFlashing)
    {
        mWindow->ClearCallbackTimer(this->flashTimer);
    }
}


/// <summary>
/// Sets the icon of this button.
/// </summary>
void TaskButton::SetIcon(HICON icon) {
    if (mNoIcons) {
        return;
    }

    mWindow->RemoveOverlay(this->icon);
    this->icon.mValid = false;
    
    if (icon == nullptr) {
        icon = LoadIcon(NULL, IDI_APPLICATION);
    }
    if (icon != nullptr) {
        this->icon = mWindow->AddOverlay(mIconRect, icon, 10);
        mWindow->Repaint();
    }
}


void TaskButton::SetOverlayIcon(HICON overlayIcon) {
    if (mNoIcons) {
        return;
    }
    
    mWindow->RemoveOverlay(this->iconOverlay);
    this->iconOverlay.mValid = false;
    if (overlayIcon != nullptr) {
        this->iconOverlay = mWindow->AddOverlay(mOverlayIconRect, overlayIcon, 20);
        mWindow->Repaint();
    }
}


/// <summary>
/// Sets the text of this button.
/// </summary>
void TaskButton::SetText(LPCWSTR title) {
    mWindow->SetText(title);
    mWindow->Repaint();
}


/// <summary>
/// Loads RC settings for task buttons.
/// </summary>
void TaskButton::LoadSettings(bool /* bIsRefresh */) {
    this->useFlashing = mSettings->GetBool(_T("UseFlashing"), true);
    this->flashInterval = mSettings->GetInt(_T("FlashInterval"), 500);
    mNoIcons = mSettings->GetBool(_T("NoIcons"), false);

    float iconSize = this->iconSettings->GetFloat(_T("Size"), 32);
    float iconX = this->iconSettings->GetFloat(_T("X"), 2);
    float iconY = this->iconSettings->GetFloat(_T("Y"), 2);
    mIconRect = D2D1::RectF(iconX, iconY, iconX + iconSize, iconY + iconSize);

    float overlayIconSize = this->iconSettings->GetFloat(_T("OverlaySize"), 16);
    float overlayIconOffsetX = this->iconSettings->GetFloat(_T("OverlayOffsetX"), 1);
    float overlayIconOffsetY = this->iconSettings->GetFloat(_T("OverlayOffsetY"), 1);

    mOverlayIconRect = D2D1::RectF(
        iconX + iconSize - overlayIconSize + overlayIconOffsetX,
        iconY + iconSize - overlayIconSize + overlayIconOffsetY,
        iconX + iconSize + overlayIconOffsetX,
        iconY + iconSize + overlayIconOffsetY);
}


/// <summary>
/// Moves and resizes the taaskbutton.
/// </summary>
void TaskButton::Reposition(UINT x, UINT y, UINT width, UINT height) {
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

    if (this->isFlashing) {
        SetFlashingState(false);
        this->flashOn = false;
        mWindow->ClearCallbackTimer(this->flashTimer);
        this->isFlashing = false;
    }
}


/// <summary>
/// Activates or deactivates the flashing state.
/// </summary>
void TaskButton::SetFlashingState(bool value) {
    if (value) {
        if (mStates[State::Hover]->active && mStates[State::Minimized]->active) {
            mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
        }
        if (mStates[State::Minimized]->active) {
            mWindow->ActivateState(mStates[State::MinimizedFlashing]);
        }
        if (mStates[State::Hover]->active) {
            mWindow->ActivateState(mStates[State::FlashingHover]);
        }
        mWindow->ActivateState(mStates[State::Flashing]);
    }
    else {
        mWindow->ClearState(mStates[State::Flashing]);
        mWindow->ClearState(mStates[State::MinimizedFlashing]);
        mWindow->ClearState(mStates[State::FlashingHover]);
        mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
    }
}


/// <summary>
/// Activates or deactivates the active state.
/// </summary>
void TaskButton::SetActiveState(bool value) {
    if (value) {
        if (mStates[State::Hover]->active) {
            mWindow->ActivateState(mStates[State::ActiveHover]);
        }
        mWindow->ActivateState(mStates[State::Active]);
    }
    else {
        mWindow->ClearState(mStates[State::Active]);
        mWindow->ClearState(mStates[State::ActiveHover]);
    }
}


/// <summary>
/// Activates or deactivates the hover state.
/// </summary>
void TaskButton::SetHoverState(bool value) {
    if (value) {
        if (mStates[State::Minimized]->active && mStates[State::Flashing]->active) {
            mWindow->ActivateState(mStates[State::MinimizedFlashingHover]);
        }
        if (mStates[State::Flashing]->active) {
            mWindow->ActivateState(mStates[State::FlashingHover]);
        }
        if (mStates[State::Minimized]->active) {
            mWindow->ActivateState(mStates[State::MinimizedHover]);
        }
        mWindow->ActivateState(mStates[State::Hover]);
    }
    else {
        mWindow->ClearState(mStates[State::Hover]);
        mWindow->ClearState(mStates[State::MinimizedHover]);
        mWindow->ClearState(mStates[State::FlashingHover]);
        mWindow->ClearState(mStates[State::MinimizedFlashingHover]);
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
void TaskButton::Flash() {
    if (!this->isFlashing) {
        this->isFlashing = true;
        this->flashOn = true;
        SetFlashingState(true);
        this->flashTimer = mWindow->SetCallbackTimer(this->flashInterval, this);
    }
}


/// <summary>
/// Shows this button.
/// </summary>
void TaskButton::Show() {
    mWindow->Show();
}


/// <summary>
/// Shows the context menu for this task button.
/// </summary>
void TaskButton::ShowMenu() {
    WINDOWPLACEMENT wp;

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(this->watchedWindow, &wp);

    // Select which menu items are enabled
    EnableMenuItem(this->menu, SC_RESTORE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(this->menu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(this->menu, SC_SIZE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(this->menu, SC_MINIMIZE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(this->menu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // let application modify menu
    PostMessage(this->watchedWindow, WM_INITMENU, (WPARAM)this->menu, 0);
    PostMessage(this->watchedWindow, WM_INITMENUPOPUP, (WPARAM)this->menu, MAKELPARAM(0, TRUE));
    
    POINT pt;
    GetCursorPos(&pt);
    
    int command = TrackPopupMenu(this->menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, mWindow->GetWindowHandle(), NULL);
    if (command != 0) {
        PostMessage(this->watchedWindow, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
}


/// <summary>
/// Places the rect of this button into the 2 POINTS structures pointed to by lpPoints.
/// </summary>
void TaskButton::GetMinRect(LPPOINTS lpPoints) {
    RECT r;
    mWindow->GetScreenRect(&r);
    lpPoints[0].x = (short)r.left;
    lpPoints[0].y = (short)r.top;
    lpPoints[1].x = (short)r.right;
    lpPoints[1].y = (short)r.bottom;
}


/// <summary>
/// Handles window messages for this button.
/// </summary>
LRESULT WINAPI TaskButton::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (message) {
    case WM_LBUTTONUP:
        {
            if (GetForegroundWindow() == this->watchedWindow)
            {
                PostMessage(this->watchedWindow, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                SetMinmizedState(true);
            }
            else if (IsIconic(this->watchedWindow))
            {
                //SetForegroundWindow(this->watchedWindow);
                BringWindowToTop(this->watchedWindow);
                PostMessage(this->watchedWindow, WM_SYSCOMMAND, SC_RESTORE, 0);
                SetForegroundWindow(this->watchedWindow);
            }
            else
            {
                SetForegroundWindow(this->watchedWindow);
            }
            ((Taskbar*)mParent)->HideThumbnail();
        }
        return 0;

    case WM_RBUTTONUP:
        {
            SetWindowPos(this->watchedWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            ShowMenu();
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            if (!this->mouseIsOver)
            {
                this->mouseIsOver = true;
                SetHoverState(true);

                RECT r;
                mWindow->GetScreenRect(&r);
                ((Taskbar*)mParent)->ShowThumbnail(this->watchedWindow, &r);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->mouseIsOver = false;
            SetHoverState(false);
            ((Taskbar*)mParent)->HideThumbnail();
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == this->flashTimer)
            {
                if (this->isFlashing)
                {
                    this->flashOn = !this->flashOn;
                    SetFlashingState(this->flashOn);
                }
                else
                {
                    mWindow->ClearCallbackTimer(this->flashTimer);
                }
            }
        }
        return 0;

    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}
