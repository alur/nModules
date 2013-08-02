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
#include "../nShared/Debugging.h"


/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(Drawable* parent, HWND watchedWindow) : Drawable(parent, "Button") {
    //
    this->iconSettings = this->settings->CreateChild("Icon");
    LoadSettings();

    this->watchedWindow = watchedWindow;

    //
    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = 0x00000000;
    stateDefaults.textOffsetLeft = 36;
    stateDefaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

    this->window->Initialize(nullptr, &stateDefaults);

    // Add states to the window
    mStates[State::Minimized] = this->window->AddState("Minimized", 25, &stateDefaults);
    mStates[State::Flashing] = this->window->AddState("Flashing", 50, &stateDefaults);
    mStates[State::MinimizedFlashing] = this->window->AddState("MinimizedFlashing", 75, &stateDefaults, &mStates[State::Flashing]);
    mStates[State::Active] = this->window->AddState("Active", 100, &stateDefaults);
    mStates[State::Hover] = this->window->AddState("Hover", 125, &stateDefaults);
    mStates[State::MinimizedHover] = this->window->AddState("MinimizedHover", 150, &stateDefaults, &mStates[State::Hover]);
    mStates[State::ActiveHover] = this->window->AddState("ActiveHover", 175, &stateDefaults, &mStates[State::Active]);
    mStates[State::FlashingHover] = this->window->AddState("FlashingHover", 200, &stateDefaults, &mStates[State::Flashing]);
    mStates[State::MinimizedFlashingHover] = this->window->AddState("MinimizedFlashingHover", 225, &stateDefaults, &mStates[State::FlashingHover]);

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
    WINDOWPLACEMENT wp;
    GetWindowPlacement(watchedWindow, &wp);
    if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) {
        SetMinmizedState(true);
    }
}


/// <summary>
/// Destructor
/// </summary>
TaskButton::~TaskButton() {
    if (this->isFlashing) {
        this->window->ClearCallbackTimer(this->flashTimer);
    }
}


/// <summary>
/// Sets the icon of this button.
/// </summary>
void TaskButton::SetIcon(HICON icon) {
    if (mNoIcons) {
        return;
    }

    this->window->RemoveOverlay(this->icon);
    this->icon.mValid = false;
    
    if (icon == nullptr) {
        icon = LoadIcon(NULL, IDI_APPLICATION);
    }
    if (icon != nullptr) {
        this->icon = this->window->AddOverlay(mIconRect, icon, 10);
        this->window->Repaint();
    }
}


void TaskButton::SetOverlayIcon(HICON overlayIcon) {
    if (mNoIcons) {
        return;
    }
    
    this->window->RemoveOverlay(this->iconOverlay);
    this->iconOverlay.mValid = false;
    if (overlayIcon != nullptr) {
        this->iconOverlay = this->window->AddOverlay(mOverlayIconRect, overlayIcon, 20);
        this->window->Repaint();
    }
}


/// <summary>
/// Sets the text of this button.
/// </summary>
void TaskButton::SetText(LPCWSTR title) {
    this->window->SetText(title);
    this->window->Repaint();
}


/// <summary>
/// Loads RC settings for task buttons.
/// </summary>
void TaskButton::LoadSettings(bool /* bIsRefresh */) {
    this->useFlashing = this->settings->GetBool("UseFlashing", true);
    this->flashInterval = this->settings->GetInt("FlashInterval", 500);
    mNoIcons = this->settings->GetBool("NoIcons", false);

    float iconSize = this->iconSettings->GetFloat("Size", 32);
    float iconX = this->iconSettings->GetFloat("X", 2);
    float iconY = this->iconSettings->GetFloat("Y", 2);
    mIconRect = D2D1::RectF(iconX, iconY, iconX + iconSize, iconY + iconSize);

    float overlayIconSize = this->iconSettings->GetFloat("OverlaySize", 16);
    float overlayIconOffsetX = this->iconSettings->GetFloat("OverlayOffsetX", 2);
    float overlayIconOffsetY = this->iconSettings->GetFloat("OverlayOffsetY", 2);

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
    this->window->SetPosition(x, y, width, height);
}


/// <summary>
/// Activates this button.
/// </summary>
void TaskButton::Activate() {
    SetActiveState(true);
    
    if (mStates[State::Minimized]->active) {
        SetMinmizedState(false);
    }

    if (this->isFlashing) {
        SetFlashingState(false);
        this->flashOn = false;
        this->window->ClearCallbackTimer(this->flashTimer);
        this->isFlashing = false;
    }
}


/// <summary>
/// Activates or deactivates the flashing state.
/// </summary>
void TaskButton::SetFlashingState(bool value) {
    if (value) {
        if (mStates[State::Hover]->active && mStates[State::Minimized]->active) {
            this->window->ActivateState(mStates[State::MinimizedFlashingHover]);
        }
        if (mStates[State::Minimized]->active) {
            this->window->ActivateState(mStates[State::MinimizedFlashing]);
        }
        if (mStates[State::Hover]->active) {
            this->window->ActivateState(mStates[State::FlashingHover]);
        }
        this->window->ActivateState(mStates[State::Flashing]);
    }
    else {
        this->window->ClearState(mStates[State::Flashing]);
        this->window->ClearState(mStates[State::MinimizedFlashing]);
        this->window->ClearState(mStates[State::FlashingHover]);
        this->window->ClearState(mStates[State::MinimizedFlashingHover]);
    }
}


/// <summary>
/// Activates or deactivates the active state.
/// </summary>
void TaskButton::SetActiveState(bool value) {
    if (value) {
        if (mStates[State::Hover]->active) {
            this->window->ActivateState(mStates[State::ActiveHover]);
        }
        this->window->ActivateState(mStates[State::Active]);
    }
    else {
        this->window->ClearState(mStates[State::Active]);
        this->window->ClearState(mStates[State::ActiveHover]);
    }
}


/// <summary>
/// Activates or deactivates the hover state.
/// </summary>
void TaskButton::SetHoverState(bool value) {
    if (value) {
        if (mStates[State::Minimized]->active && mStates[State::Flashing]->active) {
            this->window->ActivateState(mStates[State::MinimizedFlashingHover]);
        }
        if (mStates[State::Flashing]->active) {
            this->window->ActivateState(mStates[State::FlashingHover]);
        }
        if (mStates[State::Minimized]->active) {
            this->window->ActivateState(mStates[State::MinimizedHover]);
        }
        this->window->ActivateState(mStates[State::Hover]);
    }
    else {
        this->window->ClearState(mStates[State::Hover]);
        this->window->ClearState(mStates[State::MinimizedHover]);
        this->window->ClearState(mStates[State::FlashingHover]);
        this->window->ClearState(mStates[State::MinimizedFlashingHover]);
    }
}


/// <summary>
/// Activates or deactivates the minimized state.
/// </summary>
void TaskButton::SetMinmizedState(bool value) {
    if (value) {
        if (mStates[State::Hover]->active && mStates[State::Flashing]->active) {
            this->window->ActivateState(mStates[State::MinimizedFlashingHover]);
        }
        if (mStates[State::Hover]->active) {
            this->window->ActivateState(mStates[State::MinimizedHover]);
        }
        if (mStates[State::Flashing]->active) {
            this->window->ActivateState(mStates[State::MinimizedFlashing]);
        }
        this->window->ActivateState(mStates[State::Minimized]);
    }
    else {
        this->window->ClearState(mStates[State::Minimized]);
        this->window->ClearState(mStates[State::MinimizedFlashing]);
        this->window->ClearState(mStates[State::MinimizedHover]);
        this->window->ClearState(mStates[State::MinimizedFlashingHover]);
    }
}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate() {
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
        this->flashTimer = this->window->SetCallbackTimer(this->flashInterval, this);
    }
}


/// <summary>
/// Shows this button.
/// </summary>
void TaskButton::Show() {
    this->window->Show();
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
    EnableMenuItem(this->menu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(this->menu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // let application modify menu
    PostMessage(this->watchedWindow, WM_INITMENU, (WPARAM)this->menu, 0);
    PostMessage(this->watchedWindow, WM_INITMENUPOPUP, (WPARAM)this->menu, MAKELPARAM(0, TRUE));
    
    POINT pt;
    GetCursorPos(&pt);
    
    int command = TrackPopupMenu(this->menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->window->GetWindowHandle(), NULL);
    if (command != 0) {
        PostMessage(this->watchedWindow, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
}


/// <summary>
/// Places the rect of this button into the 2 POINTS structures pointed to by lpPoints.
/// </summary>
void TaskButton::GetMinRect(LPPOINTS lpPoints) {
    RECT r;
    this->window->GetScreenRect(&r);
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
            if (GetForegroundWindow() == this->watchedWindow) {
                ShowWindow(this->watchedWindow, SW_MINIMIZE);
            }
            else if (IsIconic(this->watchedWindow)) {
                ShowWindow(this->watchedWindow, SW_RESTORE);
                SetForegroundWindow(this->watchedWindow);
            }
            else {
                SetForegroundWindow(this->watchedWindow);
            }
            ((Taskbar*)this->parent)->HideThumbnail();
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
            if (!this->mouseIsOver) {
                this->mouseIsOver = true;
                SetHoverState(true);

                RECT r;
                this->window->GetScreenRect(&r);
                ((Taskbar*)this->parent)->ShowThumbnail(this->watchedWindow, &r);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->mouseIsOver = false;
            SetHoverState(false);
            ((Taskbar*)this->parent)->HideThumbnail();
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == this->flashTimer) {
                if (this->isFlashing) {
                    this->flashOn = !this->flashOn;
                    SetFlashingState(this->flashOn);
                }
                else {
                    this->window->ClearCallbackTimer(this->flashTimer);
                }
            }
        }
        return 0;

    default:
        return DefWindowProc(window, message, wParam, lParam);
    }
}
