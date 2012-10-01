/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayIcon.cpp
 *  The nModules Project
 *
 *  Implementation of the TrayIcon class. Represents a taskbar button.
 *   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/LSModule.hpp"
#include "TrayIcon.hpp"
#include "Windowsx.h"


extern LSModule* g_LSModule;
extern HWND g_hWndTrayNotify;


/// <summary>
/// Constructor
/// </summary>
TrayIcon::TrayIcon(HWND parent, LPLSNOTIFYICONDATA pNID, Settings* parentSettings) {
    //
    m_pNotifyData = pNID;

    // Create the drawable window
    this->settings = parentSettings->CreateChild("Icon");
    DrawableSettings* defaultSettings = new DrawableSettings();
    defaultSettings->color = 0x00000000;
    m_pWindow = new DrawableWindow(parent, (LPCSTR)g_LSModule->GetWindowClass(2), g_LSModule->GetInstance(), this->settings, defaultSettings, this);

    //
    LoadSettings();

    //
    UpdateIcon();
}


/// <summary>
/// Destructor
/// </summary>
TrayIcon::~TrayIcon() {
    if (m_pWindow) delete m_pWindow;
    if (this->settings) delete this->settings;
}


/// <summary>
/// Loads RC settings.
/// </summary>
void TrayIcon::LoadSettings(bool /* bIsRefresh */) {
    this->iconSize = this->settings->GetInt("Size", 16);
}


/// <summary>
/// Shows the trayicon.
/// </summary>
void TrayIcon::Show() {
    m_pWindow->Show();
}


/// <summary>
/// Retrives the icon's HWND.
/// </summary>
HWND TrayIcon::GetHWND() {
    return m_pWindow->GetWindow();
}


/// <summary>
/// Updates the icon.
/// </summary>
void TrayIcon::UpdateIcon() {
    m_pWindow->PurgeOverlays();
    if ((m_pNotifyData->uFlags & NIF_ICON) == NIF_ICON) {
        D2D1_RECT_F f;
        f.bottom = (float)this->iconSize; f.top = 0; f.left = 0; f.right = (float)this->iconSize;
        m_pWindow->AddOverlay(f, m_pNotifyData->hIcon);
    }
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
    DrawableSettings* drawableSettings = this->m_pWindow->GetSettings();
    drawableSettings->x = x;
    drawableSettings->y = y;
    drawableSettings->width = width;
    drawableSettings->height = height;
    m_pWindow->UpdatePosition();
}


/// <summary>
/// Sends a message to the owner of the icon.
/// </summary>
void TrayIcon::SendCallback(UINT uMsg, WPARAM /* wParam */, LPARAM /* lParam */) {
    if (m_pNotifyData->uVersion >= 4) {
        RECT r;
        GetWindowRect(m_pWindow->GetWindow(), &r);
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)MAKEWPARAM(r.left, r.top), (LPARAM)MAKELPARAM(uMsg, m_pNotifyData->uID));
    }
    else {
        PostMessage(m_pNotifyData->hWnd, m_pNotifyData->uCallbackMessage, (WPARAM)m_pNotifyData->uID, (LPARAM)uMsg);
    }
}


/// <summary>
/// Handles window messages for the icon's window.
/// </summary>
LRESULT WINAPI TrayIcon::HandleMessage(HWND wnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg > WM_MOUSEFIRST && uMsg < WM_MOUSELAST) {
        SendCallback(uMsg, wParam, lParam);
        if (uMsg == WM_RBUTTONUP) {
            SendCallback(WM_CONTEXTMENU, wParam, lParam);
        }
        else if (uMsg == WM_LBUTTONUP) {
            SendCallback(NIN_SELECT, wParam, lParam);
        }
    }
    else {
        return m_pWindow->HandleMessage(wnd, uMsg, wParam, lParam);
    }
    return 0;
}
