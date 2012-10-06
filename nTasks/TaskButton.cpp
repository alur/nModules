/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.cpp
 *  The nModules Project
 *
 *  Implementation of the TaskButton class. Represents a taskbar button.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "../nShared/LSModule.hpp"
#include "TaskButton.hpp"

extern LSModule* g_LSModule;

/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(Drawable* parent) : Drawable(parent, "Button") {
    //
    LoadSettings();

    this->iconSettings = this->settings->CreateChild("Icon");

    this->window->Initialize(new DrawableSettings());

    // Add states to the window
    this->stateHover = this->window->AddState("Hover", new DrawableSettings(), 100);
    this->stateActive = this->window->AddState("Active", new DrawableSettings(), 80);
    this->stateFlashing = this->window->AddState("Flashing", new DrawableSettings(), 50);

    // Initalize variables
    m_bMouseIsOver = false;
    m_bIsActive = false;
    m_hIcon = NULL;
    m_bIsFlashing = false;
}


/// <summary>
/// Destructor
/// </summary>
TaskButton::~TaskButton() {
}


/// <summary>
/// Sets the icon of this button.
/// </summary>
void TaskButton::SetIcon(HICON hIcon) {
    if (hIcon != NULL) {
        D2D1_RECT_F f = { (float)this->iconSettings->GetInt("X", 0), (float)this->iconSettings->GetInt("Y", 0),
            (float)this->iconSettings->GetInt("Width", 32) + (float)this->iconSettings->GetInt("X", 0),
            (float)this->iconSettings->GetInt("Height", 32)+ (float)this->iconSettings->GetInt("Y", 0) };
        //m_pWindow->AddOverlay(f, hIcon);
    }
    this->window->Repaint();
}


/// <summary>
/// Sets the text of this button.
/// </summary>
void TaskButton::SetText(LPCWSTR title) {
    this->window->SetText(title);
}


/// <summary>
/// Loads RC settings for task buttons.
/// </summary>
void TaskButton::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
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
    m_bIsActive = true;
    //this->window->ActivateState(this->stateActive);
    //this->window->ClearState(this->stateFlashing);
}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate() {
    m_bIsActive = false;
    //this->window->ClearState(this->stateActive);
}


/// <summary>
/// Tells this button to start flashing.
/// </summary>
void TaskButton::Flash() {
    m_bIsFlashing = true;
    //this->window->ActivateState(this->stateFlashing);
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
void TaskButton::Menu() {
    WINDOWPLACEMENT wp;
    GetSystemMenu(m_hWnd, TRUE);
    hMenu = GetSystemMenu(m_hWnd, FALSE);

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(m_hWnd, &wp);

    // restore is enabled only if minimized or maximized (not normal)
    EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // move is enabled only if normal
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // size is enabled only if normal
    EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // minimize is enabled only if not minimized
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));

    // maximize is enabled only if not maximized
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // let application modify menu
    PostMessage(m_hWnd, WM_INITMENUPOPUP, (WPARAM)hMenu, MAKELPARAM(0, TRUE));
    PostMessage(m_hWnd, WM_INITMENU, (WPARAM)hMenu, 0);
    
    POINT pt;
    GetCursorPos(&pt);
    
    int command = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, NULL, NULL);
    if (command != 0) {
        PostMessage(m_hWnd, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
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
LRESULT WINAPI TaskButton::HandleMessage(HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONUP:
        if (GetForegroundWindow() == m_hWnd) {
            ShowWindow(m_hWnd, SW_MINIMIZE);
        }
        else if (IsIconic(m_hWnd)) {
            SetForegroundWindow(m_hWnd);
            ShowWindow(m_hWnd, SW_RESTORE);
        }
        else {
            SetForegroundWindow(m_hWnd);
        }
        return 0;

    case WM_RBUTTONUP:
        SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
        Menu();
        return 0;

    case WM_MOUSEMOVE:
        if (!m_bMouseIsOver) {
            m_bMouseIsOver = true;
            TrackMouseEvent(&m_TrackMouseStruct);
            this->window->ActivateState(this->stateHover);
        }
        return 0;

    case WM_MOUSELEAVE:
        m_bMouseIsOver = false;
        this->window->ClearState(this->stateHover);
        return 0;

    default:
        return window->HandleMessage(wnd, uMsg, wParam, lParam);
    }
}
