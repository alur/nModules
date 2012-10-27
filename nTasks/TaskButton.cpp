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

extern LSModule* g_LSModule;

/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(Drawable* parent, HWND watchedWindow) : Drawable(parent, "Button") {
    //
    LoadSettings();

    this->watchedWindow = watchedWindow;

    this->iconSettings = this->settings->CreateChild("Icon");

    this->window->Initialize(new DrawableSettings(), new DrawableStateSettings());

    // Add states to the window
    this->stateFlashingHover = this->window->AddState("FlashingHover", new DrawableStateSettings(), 150); 
    this->stateActiveHover = this->window->AddState("ActiveHover", new DrawableStateSettings(), 125);
    this->stateHover = this->window->AddState("Hover", new DrawableStateSettings(), 100);
    this->stateActive = this->window->AddState("Active", new DrawableStateSettings(), 75);
    this->stateFlashing = this->window->AddState("Flashing", new DrawableStateSettings(), 50);

    // Initalize variables
    this->mouseIsOver = false;
    this->isFlashing = false;

    // Reset the system menu for the window
    GetSystemMenu(this->watchedWindow, TRUE);
    this->menu = GetSystemMenu(this->watchedWindow, FALSE);
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
    this->window->ClearOverlays();
    if (icon != NULL) {
        D2D1_RECT_F f = { (float)this->iconSettings->GetInt("X", 0), (float)this->iconSettings->GetInt("Y", 0),
            (float)this->iconSettings->GetInt("Width", 32) + (float)this->iconSettings->GetInt("X", 0),
            (float)this->iconSettings->GetInt("Height", 32)+ (float)this->iconSettings->GetInt("Y", 0) };
        this->iconOverlay = this->window->AddOverlay(f, icon);
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
    if (this->stateHover->active) {
        this->window->ActivateState(this->stateActiveHover);
    }
    this->window->ActivateState(this->stateActive);

    if (this->isFlashing) {
        this->window->ClearState(this->stateFlashing);
        this->window->ClearState(this->stateFlashingHover);
        this->flashOn = false;
        this->window->ClearCallbackTimer(this->flashTimer);
        this->isFlashing = false;
    }
}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate() {
    this->window->ClearState(this->stateActive);
    this->window->ClearState(this->stateActiveHover);
}


/// <summary>
/// Tells this button to start flashing.
/// </summary>
void TaskButton::Flash() {
    if (!this->isFlashing) {
        this->isFlashing = true;
        this->flashOn = true;
        if (this->stateHover->active) {
            this->window->ActivateState(this->stateFlashingHover);
        }
        this->window->ActivateState(this->stateFlashing);
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
    PostMessage(this->watchedWindow, WM_INITMENUPOPUP, (WPARAM)this->menu, MAKELPARAM(0, TRUE));
    PostMessage(this->watchedWindow, WM_INITMENU, (WPARAM)this->menu, 0);
    
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

    TRACEW(L"GetMinRect() %d %d %d %d -- %s", r.left, r.top, r.right, r.bottom, this->window->GetDrawingSettings()->text);
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

                RECT r;
                this->window->GetScreenRect(&r);
                ((Taskbar*)this->parent)->ShowThumbnail(this->watchedWindow, &r);

                if (this->stateActive->active) {
                    this->window->ActivateState(this->stateActiveHover);
                }

                if (this->stateFlashing->active) {
                    this->window->ActivateState(this->stateFlashingHover);
                }

                this->window->ActivateState(this->stateHover);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->mouseIsOver = false;
            this->window->ClearState(this->stateHover);
            this->window->ClearState(this->stateActiveHover);
            this->window->ClearState(this->stateFlashingHover);
            ((Taskbar*)this->parent)->HideThumbnail();
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == this->flashTimer) {
                if (this->isFlashing) {
                    this->flashOn = !this->flashOn;
                    if (this->flashOn) {
                        if (this->stateHover->active) {
                            this->window->ActivateState(this->stateFlashingHover);
                        }
                        this->window->ActivateState(this->stateFlashing);
                    }
                    else {
                        this->window->ClearState(this->stateFlashing);
                        this->window->ClearState(this->stateFlashingHover);
                    }
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
