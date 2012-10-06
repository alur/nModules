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
TrayIcon::TrayIcon(Drawable* parent, LPLSNOTIFYICONDATA pNID, Settings* parentSettings) : Drawable(parent, "Icon") {
    //
    m_pNotifyData = pNID;

    // Create the drawable window
    this->settings = parentSettings->CreateChild("Icon");
    DrawableSettings* defaultSettings = new DrawableSettings();
    defaultSettings->color = 0x00000000;
    this->window->Initialize(defaultSettings);

    //
    LoadSettings();

    //
    UpdateIcon();
}


/// <summary>
/// Destructor
/// </summary>
TrayIcon::~TrayIcon() {
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
    this->window->Show();
}


/// <summary>
/// Updates the icon.
/// </summary>
void TrayIcon::UpdateIcon() {
    if ((m_pNotifyData->uFlags & NIF_ICON) == NIF_ICON) {
        D2D1_RECT_F f;
        f.bottom = (float)this->iconSize; f.top = 0; f.left = 0; f.right = (float)this->iconSize;
        //m_pWindow->AddOverlay(f, m_pNotifyData->hIcon);
    }
}


/// <summary>
/// Repositions the icon.
/// </summary>
void TrayIcon::Reposition(UINT x, UINT y, UINT width, UINT height) {
    this->window->SetPosition(x, y, width, height);
}


/// <summary>
/// Gets the screen coordinate rect of this tray icon.
/// </summary>
void TrayIcon::GetScreenRect(LPRECT rect) {
    this->window->GetScreenRect(rect);
}


/// <summary>
/// Sends a message to the owner of the icon.
/// </summary>
void TrayIcon::SendCallback(UINT uMsg, WPARAM /* wParam */, LPARAM /* lParam */) {
    if (m_pNotifyData->uVersion >= 4) {
        RECT r;
        this->window->GetScreenRect(&r);
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
        return this->window->HandleMessage(wnd, uMsg, wParam, lParam);
    }
    return 0;
}
