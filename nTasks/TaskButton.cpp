/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.cpp                                                      July, 2012
 *  The nModules Project
 *
 *  Implementation of the TaskButton class. Represents a taskbar button.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "TaskButton.hpp"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTaskButtonHandler;


/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(HWND parent, HWND window, LPCSTR prefix) {
    m_pIconPaintSettings = NULL;

    // Generate our prefixes
    char szPrefix[512];
    sprintf_s(szPrefix, sizeof(szPrefix), "%s%s", prefix, "Button");
    m_pszPrefix = _strdup(szPrefix);
    sprintf_s(szPrefix, sizeof(szPrefix), "%s%s", prefix, "ButtonActive");
    m_pszPrefixActive = _strdup(szPrefix);
    sprintf_s(szPrefix, sizeof(szPrefix), "%s%s", prefix, "ButtonHover"); // TODO::The drawable window should handle this automatically.
    m_pszPrefixHover = _strdup(szPrefix);
    sprintf_s(szPrefix, sizeof(szPrefix), "%s%s", prefix, "ButtonFlashing");
    m_pszPrefixFlashing = _strdup(szPrefix);
    sprintf_s(szPrefix, sizeof(szPrefix), "%s%s", prefix, "ButtonIcon");
    m_pszPrefixIcon = _strdup(szPrefix);

    // 
    m_hWnd = window;
    m_hWndParent = parent;

    //
    LoadSettings();

    // Create the drawable window
    m_pPaintSettings = new PaintSettings(m_pszPrefix);
    m_pWindow = new DrawableWindow(parent, g_szTaskButtonHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);

    m_pIconPaintSettings = new PaintSettings(m_pszPrefixIcon);

    // Configure the mouse tracking struct
    ZeroMemory(&m_TrackMouseStruct, sizeof(TRACKMOUSEEVENT));
    m_TrackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    m_TrackMouseStruct.hwndTrack = m_pWindow->getWindow();
    m_TrackMouseStruct.dwFlags = TME_LEAVE;
    m_TrackMouseStruct.dwHoverTime = 200;

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
    if (m_pWindow) delete m_pWindow;
    if (m_pPaintSettings) delete m_pPaintSettings;
    free((LPVOID)m_pszPrefix);
    free((LPVOID)m_pszPrefixActive);
    free((LPVOID)m_pszPrefixHover);
    free((LPVOID)m_pszPrefixIcon);
}


void TaskButton::SetIcon(HICON hIcon) {
    m_pWindow->PurgeOverlays();
    if (hIcon != NULL) {
        D2D1_RECT_F f = { m_pIconPaintSettings->position.left, m_pIconPaintSettings->position.top,
            m_pIconPaintSettings->position.right - m_pIconPaintSettings->position.left,
            m_pIconPaintSettings->position.bottom - m_pIconPaintSettings->position.top };
        m_pWindow->AddOverlay(f, hIcon);
    }
    m_pWindow->Repaint();
}


void TaskButton::SetText(LPCWSTR pszTitle) {
    m_pPaintSettings->setText(pszTitle);
    m_pWindow->Repaint();
}


void TaskButton::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
}


void TaskButton::Reposition(UINT x, UINT y, UINT width, UINT height) {
    m_pPaintSettings->position.left = x;
    m_pPaintSettings->position.top = y;
    m_pPaintSettings->position.right = x + width;
    m_pPaintSettings->position.bottom = y + height;
    m_pWindow->UpdatePosition();
}


void TaskButton::Activate() {
    m_bIsActive = true;
    m_pPaintSettings->OverLoad(m_pszPrefixActive);
    m_pWindow->UpdateBrushes();
}


void TaskButton::Deactivate() {
    m_bIsActive = false;
    m_pPaintSettings->OverLoad(m_pszPrefix);
    m_pWindow->UpdateBrushes();
}


void TaskButton::Flash() {
    m_bIsFlashing = true;
    m_pPaintSettings->OverLoad(m_pszPrefixFlashing);
    m_pWindow->UpdateBrushes();
}


void TaskButton::Show() {
    m_pWindow->Show();
}


void TaskButton::Menu() {
    WINDOWPLACEMENT wp;

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
    GetCursorPos( &pt );
    
    int command = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_pWindow->getWindow(), NULL);
    if (command != 0) {
        PostMessage(m_hWnd, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
}


void TaskButton::GetMinRect(LPPOINTS lpPoints) {
    RECT r;
    GetWindowRect(m_pWindow->getWindow(), &r);
    lpPoints[0].x = (short)r.left;
    lpPoints[0].y = (short)r.top;
    lpPoints[1].x = (short)r.right;
    lpPoints[1].y = (short)r.bottom;
}


LRESULT WINAPI TaskButton::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
            m_pPaintSettings->OverLoad(m_pszPrefixHover);
            m_pWindow->UpdateBrushes();
        }
        return 0;

    case WM_MOUSELEAVE:
        m_bMouseIsOver = false;
        m_pPaintSettings->OverLoad(m_bIsActive  ? m_pszPrefixActive : m_pszPrefix);
        m_pWindow->UpdateBrushes();
        return 0;

    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
